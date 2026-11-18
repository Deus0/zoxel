// each voxel's part while combining (its entry + 1, 0 air), a flat grid the body's size: its lookups are array reads,
//  the octree walks made the combine 2.4 times slower; written to the BoneNode once at the end
typedef struct {
    byte* cells;
    int3 size;
} combine_parts;

static inline byte combine_parts_get(const combine_parts* parts, int3 p) {
    if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= parts->size.x || p.y >= parts->size.y || p.z >= parts->size.z) {
        return 0;
    }
    return parts->cells[(p.x * parts->size.y + p.y) * parts->size.z + p.z];
}

static inline void combine_parts_set(combine_parts* parts, int3 p, byte part) {
    parts->cells[(p.x * parts->size.y + p.y) * parts->size.z + p.z] = part;
}

// clothes voxels past the body, their part found after every part is placed
typedef struct {
    int3* positions;
    byte* entries;
    int count;
    int capacity;
} combine_parts_pending;

#define combine_part_unset 255

// a voxel's part: its entry's, or for clothes (over), the part it covers, else found later (the nearest body part)
static inline void combine_part_set(combine_parts* parts, combine_parts_pending* pending, int3 position, byte entry, byte is_over) {
    if (!is_over) {
        combine_parts_set(parts, position, entry + 1);
        return;
    }
    // over a part already (or waiting already)
    if (combine_parts_get(parts, position)) {
        return;
    }
    if (pending->count == pending->capacity) {
        pending->capacity = pending->capacity ? pending->capacity * 2 : 64;
        pending->positions = realloc(pending->positions, pending->capacity * sizeof(int3));
        pending->entries = realloc(pending->entries, pending->capacity);
    }
    pending->positions[pending->count] = position;
    pending->entries[pending->count++] = entry;
    combine_parts_set(parts, position, combine_part_unset);
}

// two parts (entries + 1) joined: the same, or one the other's parent (CombineParents); any if there are no parents
static inline byte combine_parts_joined(const CombineParents* parents, byte a, byte b) {
    if (a == b || !parents) {
        return 1;
    }
    const byte parent_a = a - 1 < parents->length ? parents->value[a - 1] : 0;
    const byte parent_b = b - 1 < parents->length ? parents->value[b - 1] : 0;
    return parent_a == b || parent_b == a;
}

// a part's voxel there would touch (or be over) a part it isn't joined to: left out, a gap between them, so neither
//  part's faces there are culled and each skin stays closed as they move apart (the faces between touching parts
//  were kept before, many unseen ones, tst body_poses)
static inline byte combine_part_touches(const combine_parts* parts, const CombineParents* parents, int3 p, byte part) {
    for (byte d = 0; d < 7; d++) {
        const byte other = combine_parts_get(parts, d == 6 ? p : int3_add(p, get_direction_int3(d)));
        if (other && other != combine_part_unset && !combine_parts_joined(parents, part, other)) {
            return 1;
        }
    }
    return 0;
}

// a clothes voxel's part: the most of its 6 neighbours' that are set (ties: the lower entry), 0 if none are
static inline byte combine_part_neighbours(const combine_parts* parts, int3 p) {
    byte found[6];
    byte counts[6];
    byte kinds = 0;
    for (byte d = 0; d < 6; d++) {
        const byte part = combine_parts_get(parts, int3_add(p, get_direction_int3(d)));
        if (!part || part == combine_part_unset) {
            continue;
        }
        byte k = 0;
        while (k < kinds && found[k] != part) {
            k++;
        }
        if (k == kinds) {
            found[kinds] = part;
            counts[kinds++] = 0;
        }
        counts[k]++;
    }
    byte best = 0;
    for (byte k = 0; k < kinds; k++) {
        if (!best || counts[k] > counts[best - 1] || (counts[k] == counts[best - 1] && found[k] < found[best - 1])) {
            best = k + 1;
        }
    }
    return best ? found[best - 1] : 0;
}

// clothes voxels past the body take the part of the body they're nearest to through the clothes: a flood out from
//  the body a layer at a time (a sleeve's outside its arm's, the shorts' legs their thighs'); each layer reads only the
//  layers before it, so a part can't crawl along the clothes in the loop's order. Not touching the body (through the
//  clothes), its own entry (CombineBones gives it its parent's bone)
static inline void combine_parts_resolve(combine_parts* parts, VoxelNode* voxels, const CombineParents* parents, combine_parts_pending* pending, byte depth) {
    byte* layer = malloc(pending->count ? pending->count : 1);
    byte changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < pending->count; i++) {
            const int3 p = pending->positions[i];
            // a part placed after it took it (a leg after a glove), or resolved a layer before
            layer[i] = combine_parts_get(parts, p) != combine_part_unset ? 0 : combine_part_neighbours(parts, p);
        }
        for (int i = 0; i < pending->count; i++) {
            if (layer[i]) {
                combine_parts_set(parts, pending->positions[i], layer[i]);
                changed = 1;
            }
        }
    }
    for (int i = 0; i < pending->count; i++) {
        if (combine_parts_get(parts, pending->positions[i]) == combine_part_unset) {
            combine_parts_set(parts, pending->positions[i], pending->entries[i] + 1);
        }
    }
    // then clothes touching a part they aren't joined to are left out (both sides, so it's the same either way round)
    for (int i = 0; i < pending->count; i++) {
        const byte part = combine_parts_get(parts, pending->positions[i]);
        layer[i] = part && combine_part_touches(parts, parents, pending->positions[i], part);
    }
    for (int i = 0; i < pending->count; i++) {
        if (layer[i]) {
            set_VoxelNode(voxels, depth, int3_to_byte3(pending->positions[i]), 0);
            combine_parts_set(parts, pending->positions[i], 0);
        }
    }
    free(layer);
    free(pending->positions);
    free(pending->entries);
}

// the grid written to the BoneNode, its voxels' parts
static inline void combine_parts_write(const combine_parts* parts, BoneNode* nodes, byte depth) {
    int3 p;
    for (p.x = 0; p.x < parts->size.x; p.x++) {
        for (p.y = 0; p.y < parts->size.y; p.y++) {
            for (p.z = 0; p.z < parts->size.z; p.z++) {
                const byte part = combine_parts_get(parts, p);
                if (part) {
                    set_BoneNode(nodes, depth, int3_to_byte3(p), part);
                }
            }
        }
    }
}

void combine_vox_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    ushort max_colors = 255;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CombineVox);
    zox_sys_in(CombineList);
    zox_sys_in(CombinePositions);
    zox_sys_out(ChunkSize);
    zox_sys_out(NodeDepth);
    zox_sys_out(RenderDepth);
    zox_sys_out(VoxelNode);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CombineVox, state);
        zox_sys_i(CombineList, voxes);
        zox_sys_i(CombinePositions, positions);
        zox_sys_o(ChunkSize, csize);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(RenderDepth, rdepth);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(ColorRGBs, colors);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (voxes->length != positions->length) {
            zox_loge("Vox and Positions out of Sync, cannot combine.");
            continue;
        }
        // zox_log("Combining Voxes [%i]", voxes->length);
        // NOTE: Optional depths to sample each vox at, as models only store max depth voxels
        const CombineDepths* depths = zox_has(e, CombineDepths) ?
            zox_get(e, CombineDepths) : NULL;
        if (depths && depths->length != voxes->length) {
            depths = NULL;
        }
        // TODO: calculate the overall size first
        // add each vox to the current node
        int3 new_csize = int3_zero;
        // set voctree here
        resize_ColorRGBs(colors, 0);
        // TODO: set voctree depth and set to air
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        // Clear octree
        voctree->value = 0;
        collapse_VoxelNode(voctree);
        // each voxel's part, for its bone (BoneNode, doc/done/skeletons.md)
        BoneNode* bone_nodes = zox_has(e, BoneNode) ? zox_getm(e, BoneNode) : NULL;
        const CombineOver* over = zox_has(e, CombineOver) ? zox_get(e, CombineOver) : NULL;
        if (over && over->length != voxes->length) {
            over = NULL;
        }
        combine_parts_pending pending = { 0 };
        const CombineParents* parents = zox_has(e, CombineParents) ? zox_get(e, CombineParents) : NULL;
        if (parents && parents->length != voxes->length) {
            parents = NULL;
        }
        combine_parts parts_grid = { 0 };
        combine_parts* parts = NULL;
        if (bone_nodes) {
            bone_nodes->value = 0;
            collapse_BoneNode(bone_nodes);
            // the body's size from its entries (each placed at its position, its model's size)
            for (byte j = 0; j < voxes->length; j++) {
                const entity vox = voxes->value[j];
                if (!zox_valid(vox) || !zox_has(vox, ChunkSize) || !zox_has(vox, NodeDepth)) {
                    continue;
                }
                int3 vox_size = zox_getv(vox, ChunkSize);
                if (depths && depths->value[j] < zox_getv(vox, NodeDepth)) {
                    vox_size = get_vox_size_at_depth(vox_size, zox_getv(vox, NodeDepth), depths->value[j]);
                }
                parts_grid.size = int3_max_each(parts_grid.size, int3_add(byte3_to_int3(positions->value[j]), vox_size));
            }
            const int length = octree_size(ndepth->value);
            parts_grid.size = int3_min_each(parts_grid.size, (int3) { length, length, length });
            parts_grid.cells = calloc((size_t) parts_grid.size.x * parts_grid.size.y * parts_grid.size.z + 1, 1);
            parts = &parts_grid;
        }
        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];
            if (!zox_valid(vox)) {
                continue;
            }
            if (!zox_has(vox, NodeDepth)) {
                zox_loge("Combining Vox [%s] has no [NodeDepth]", zox_get_name(vox));
                continue;
            }
            if (!zox_has(vox, ChunkSize)) {
                zox_loge("Combining Vox [%s] has no [ChunkSize]", zox_get_name(vox));
                continue;
            }
            /*if (ndepth->value != zox_getv(vox, NodeDepth)) {
                zox_logw("We don't support adding different depths yet");
                continue;
            }*/
            // NOTE: Colors, we will need to make a lookup table for index placement
            byte3 vposition = positions->value[j];
            byte vox_depth = zox_getv(vox, NodeDepth);
            int3 vox_size = zox_getv(vox, ChunkSize);
            if (depths && depths->value[j] < vox_depth) {
                vox_size = get_vox_size_at_depth(vox_size, vox_depth, depths->value[j]);
                vox_depth = depths->value[j];
            }
            const VoxelNode* vox_octree = zox_get(vox, VoxelNode);
            const ColorRGBs* vox_colors = zox_get(vox, ColorRGBs);
            // get max octree type
            byte max_color_index = get_max_value_VoxelNode(vox_octree);
            if (dbg_log) {
                zox_log("Vox [%i] max_color_index for vox [%s] [%i]", j, zox_get_name(vox), max_color_index);
            }
            for (ushort k = 0; k < max_color_index && colors->length < max_colors; k++) {
                // NOTE: Added check to make sure color is actually inside vox, for imported voxes
                byte is_in = is_in_VoxelNode(vox_octree, k + 1);
                if (!is_in) {
                    continue;
                }
                color_rgb acolor = vox_colors->value[k];
                // If not in list
                byte already_added = 0;
                for (ushort l = 0; l < colors->length; l++) {
                    color_rgb base_color = colors->value[l];
                    if (color_rgb_equals(base_color, acolor)) {
                        already_added = 1;
                        break;
                    }
                }
                if (!already_added) {
                    add_to_ColorRGBs(colors, acolor);
                }
            }
            // NOTE: For now lets assume same depth
            // add aoctree into voctree
            // account for depth difference
            // position, size for placement into new grid?
            // combine colors too
            ushort3 max_placement = (ushort3) {
                vposition.x + vox_size.x,
                vposition.y + vox_size.y,
                vposition.z + vox_size.z
            };
            ushort3 lposition = ushort3_zero;
            ushort3 position;
            for (lposition.x = 0, position.x = vposition.x; position.x < max_placement.x; position.x++, lposition.x++) {
                for (lposition.y = 0, position.y = vposition.y; position.y < max_placement.y; position.y++, lposition.y++) {
                    for (lposition.z = 0, position.z = vposition.z; position.z < max_placement.z; position.z++, lposition.z++) {
                        byte place_vox_value = getv_VoxelNode(vox_octree, vox_depth, ushort3_to_byte3(lposition));
                        if (!place_vox_value) {
                            continue;
                        }
                        // Convert to our placement
                        // get color from value
                        color_rgb place_vox_color = vox_colors->value[place_vox_value - 1];
                        // find color in place vox
                        byte value = 0;
                        // TODO: Use Colors Dictionary!
                        for (ushort k = 0; k < colors->length; k++) {
                            color_rgb body_color = colors->value[k];
                            if (color_rgb_equals(body_color, place_vox_color)) {
                                value = k + 1;  // + 1 for air
                                break;
                            }
                        }
                        // didn't find, maybe over max?
                        if (!value) {
                            continue;
                        }
                        byte3 positionb = ushort3_to_byte3(position);
                        const byte is_over = over && over->value[j];
                        if (parts && !is_over && combine_part_touches(parts, parents, ushort3_to_int3(position), j + 1)) {
                            continue;
                        }
                        set_VoxelNode(voctree, ndepth->value, positionb, value);
                        if (parts) {
                            combine_part_set(parts, &pending, ushort3_to_int3(position), j, is_over);
                        }
                        // Expands the size of our vox
                        if (position.x >= new_csize.x) {
                            new_csize.x = position.x;
                        }
                        if (position.y >= new_csize.y) {
                            new_csize.y = position.y;
                        }
                        if (position.z >= new_csize.z) {
                            new_csize.z = position.z;
                        }
                        // zox_log("At [%ix%ix%i] placed [%i] - from [%ix%ix%i]", position.x, position.y, position.z, value, gposition.x, gposition.y, gposition.z);
                    }
                }
            }
            // zox_log(" + vox %i [%s] c[%i] at [%ix%ix%i] of s[%ix%ix%i]", j, zox_get_name(vox), acolors->length, vposition.x, vposition.y, vposition.z, vox_size.x, vox_size.y, vox_size.z);
        }
        if (parts) {
            combine_parts_resolve(parts, voctree, parents, &pending, ndepth->value);
            combine_parts_write(parts, bone_nodes, ndepth->value);
            optimize_reduce_BoneNode(bone_nodes);
            free(parts->cells);
        }
        zox_octree_lock_end(write_lock);
        csize->value = new_csize;
        // NOTE: Vox data is always at max depth, its owners RenderDepth picks the mesh
        rdepth->value = ndepth->value;
        zox_add(e, VoxelNodeDirty);
        /*zox_log(" - colors [%i]", colors->length);
        zox_log(" - vdepth [%i] - grid max [%i]", ndepth->value, powers_of_two_byte[ndepth->value]);
        zox_log(" - csize [%ix%ix%i]", csize->value.x, csize->value.y, csize->value.z);
        zox_log(" - bscale [%f]", bscale->value);*/
    }
    zox_sys_on_end();
} zoxd_system(combine_vox_system);

        /*byte new_depth = 0;
        // float new_scale = 0;
        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];
            if (!zox_valid(vox)) {
                continue;
            }
            byte vdepth = zox_getv(vox, NodeDepth);
            if (vdepth > new_depth) {
                new_depth = vdepth;
            }
            if (vdepth == new_depth) {
                int3 vox_size = zox_getv(vox, ChunkSize);
                if (vox_size.x > new_csize.x) new_csize.x = vox_size.x;
                if (vox_size.y > new_csize.y) new_csize.y = vox_size.y;
                if (vox_size.z > new_csize.z) new_csize.z = vox_size.z;
            }
        }
        ndepth->value = new_depth;*/
