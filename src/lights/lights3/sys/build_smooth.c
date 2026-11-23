// Smooth lighting: each vertex of a face is lit by the air around it, not the face's one light
//  - a vertex averages four lights: the face's (the voxel it looks into), its two edges' and its corner's
//  - a corner behind two solid edges is hidden, its light would leak round the corner, it counts as darklight
//  - solids hold darklight, so vertices in creases darken (ambient occlusion)
//  - walks the voxels (node) with the sides like the mesh builder (zox_terrain_building_dig), so both skip the
//    same faces (sides set on air), and colors line up with the mesh's vertices
//  - a face into a door's part (light_solidity_open or _base: it stops light while shut, it's no cube) is lit flat by
//    the darkest open air round it (smooth_seal_light): its 3 x 3 reached past the door's plane, a wall's side by a shut
//    door took the sunlit outside's light at its edge while the room was dark (as block voxes, vodes3/fun/lights.c)
//  - tst smooth_lights (the corner leak), terrain_render, door_light (the wall beside a shut door)

// meshes found older than their chunk's sides (more faces to light than vertices), rebuilt: the sides changed after it
//  was built, its BuildMesh not merged yet (it logged Colors Past Limits as an error), counted for tests
uint zox_smooth_lights_stale = 0;

// a face's two axes along it, a vertex is one step along each (signed by smooth_vertex_signs)
static const sbyte3 smooth_face_axes[6][2] = {
    [direction_left] = { { 0, 1, 0 }, { 0, 0, 1 } },
    [direction_right] = { { 0, 1, 0 }, { 0, 0, 1 } },
    [direction_down] = { { 1, 0, 0 }, { 0, 0, 1 } },
    [direction_up] = { { 1, 0, 0 }, { 0, 0, 1 } },
    [direction_back] = { { 1, 0, 0 }, { 0, 1, 0 } },
    [direction_front] = { { 1, 0, 0 }, { 0, 1, 0 } },
};

// the mesh's vertex order on a face: --, -+, ++, +-
static const sbyte smooth_vertex_signs[4][2] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };

// a vertex's light: the face's, its edges' and its corner's averaged, a corner behind two solid edges is darklight
static inline byte smooth_vertex_light(byte face, byte edge_a, byte edge_b, byte corner, byte solid_a, byte solid_b) {
    if (solid_a && solid_b) {
        corner = darklight;
    }
    return (byte) ((face + edge_a + edge_b + corner) / 4);
}

// does the voxel at offset from position stop light (missing chunks are air)
static inline byte smooth_is_solid(
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    const byte* solidity,
    byte3 position,
    byte depth,
    sbyte3 offset)
{
    const byte voxel = getv_nearby_VoxelNode(voxels, voxels_locks, position, depth, offset, 0);
    return voxel && solidity[voxel - 1] && solidity[voxel - 1] != light_solidity_base;
}

// a door's part: stops light only while shut (or over a shut door), drawn as a block vox, not a cube
static inline byte smooth_is_seal(const byte* solidity, byte voxel) {
    return voxel && (solidity[voxel - 1] == light_solidity_open || solidity[voxel - 1] == light_solidity_base);
}

// a face into a door's part: the darkest of the open air in the 3 x 3 across it (not solid, not a door's part), else
//  the part's own light; open[sa][sb] says which pass light
static inline byte smooth_seal_light(byte light[3][3], byte open[3][3]) {
    byte darkest = 255, found = 0;
    for (byte x = 0; x < 3; x++)
    for (byte y = 0; y < 3; y++) {
        if ((x != 1 || y != 1) && open[x][y] && light[x][y] < darkest) {
            darkest = light[x][y];
            found = 1;
        }
    }
    return found ? darkest : light[1][1];
}

static inline sbyte3 smooth_offset(sbyte3 face, sbyte3 a, sbyte sign_a, sbyte3 b, sbyte sign_b) {
    return (sbyte3) {
        face.x + a.x * sign_a + b.x * sign_b,
        face.y + a.y * sign_a + b.y * sign_b,
        face.z + a.z * sign_a + b.z * sign_b,
    };
}

// a face's four vertex lights, in the mesh's vertex order, for the voxel at position (27 nearby chunks)
void smooth_face_lights(
    const LightNode** lights,
    spinlock** lights_locks,
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    const byte* solidity,
    byte3 position,
    byte depth,
    byte direction,
    byte* vertex_lights)
{
    // past the world: the sky above, dark elsewhere
    const byte oob = direction == direction_up ? sunlight : darklight;
    const sbyte3 face = neighbor_offsets[direction];
    const sbyte3 a = smooth_face_axes[direction][0];
    const sbyte3 b = smooth_face_axes[direction][1];
    // the 3 x 3 lights across the face and the 4 solids beside it, read once: the four vertices share them (it read
    //  21 a face, 13 now, each a walk down a tree)
    byte light[3][3], solid_a[2], solid_b[2];
    for (sbyte sa = -1; sa <= 1; sa++)
    for (sbyte sb = -1; sb <= 1; sb++) {
        light[sa + 1][sb + 1] = getv_nearby_max_LightNode(lights, lights_locks, position, depth, smooth_offset(face, a, sa, b, sb), oob);
    }
    if (smooth_is_seal(solidity, getv_nearby_VoxelNode(voxels, voxels_locks, position, depth, face, 0))) {
        byte open[3][3];
        for (sbyte sa = -1; sa <= 1; sa++)
        for (sbyte sb = -1; sb <= 1; sb++) {
            const byte voxel = getv_nearby_VoxelNode(voxels, voxels_locks, position, depth, smooth_offset(face, a, sa, b, sb), 0);
            open[sa + 1][sb + 1] = !voxel || !solidity[voxel - 1];
        }
        const byte sealed = smooth_seal_light(light, open);
        for (byte v = 0; v < 4; v++) {
            vertex_lights[v] = sealed;
        }
        return;
    }
    for (byte i = 0; i < 2; i++) {
        const sbyte sign = i ? 1 : -1;
        solid_a[i] = smooth_is_solid(voxels, voxels_locks, solidity, position, depth, smooth_offset(face, a, sign, b, 0));
        solid_b[i] = smooth_is_solid(voxels, voxels_locks, solidity, position, depth, smooth_offset(face, a, 0, b, sign));
    }
    for (byte v = 0; v < 4; v++) {
        const sbyte sa = smooth_vertex_signs[v][0], sb = smooth_vertex_signs[v][1];
        vertex_lights[v] = smooth_vertex_light(light[1][1], light[sa + 1][1], light[1][sb + 1], light[sa + 1][sb + 1],
            solid_a[sa > 0], solid_b[sb > 0]);
    }
}

static inline byte smooth_grid_light(const smooth_grid* grid, byte3 position, sbyte3 offset, byte oob) {
    const int x = position.x + offset.x + 1, y = position.y + offset.y + 1, z = position.z + offset.z + 1;
    const int last = grid->length - 1;
    if (!grid->present[OCTREE_INDEX(!x ? -1 : x == last, !y ? -1 : y == last, !z ? -1 : z == last)]) {
        return oob;
    }
    return grid->lights[(x * grid->length + y) * grid->length + z];
}

static inline byte smooth_grid_voxel(const smooth_grid* grid, byte3 position, sbyte3 offset) {
    return grid->voxels[((position.x + offset.x + 1) * grid->length + position.y + offset.y + 1) * grid->length +
        position.z + offset.z + 1];
}

static inline byte smooth_grid_solid(const smooth_grid* grid, const byte* solidity, byte3 position, sbyte3 offset) {
    const byte voxel = smooth_grid_voxel(grid, position, offset);
    return voxel && solidity[voxel - 1] && solidity[voxel - 1] != light_solidity_base;
}

// smooth_face_lights from the grid, the same lights (tst smooth_lights compares them)
static inline void smooth_face_lights_grid(
    const smooth_grid* grid,
    const byte* solidity,
    byte3 position,
    byte direction,
    byte* vertex_lights)
{
    const byte oob = direction == direction_up ? sunlight : darklight;
    const sbyte3 face = neighbor_offsets[direction];
    const sbyte3 a = smooth_face_axes[direction][0];
    const sbyte3 b = smooth_face_axes[direction][1];
    byte light[3][3], solid_a[2], solid_b[2];
    for (sbyte sa = -1; sa <= 1; sa++)
    for (sbyte sb = -1; sb <= 1; sb++) {
        light[sa + 1][sb + 1] = smooth_grid_light(grid, position, smooth_offset(face, a, sa, b, sb), oob);
    }
    if (smooth_is_seal(solidity, smooth_grid_voxel(grid, position, face))) {
        byte open[3][3];
        for (sbyte sa = -1; sa <= 1; sa++)
        for (sbyte sb = -1; sb <= 1; sb++) {
            const byte voxel = smooth_grid_voxel(grid, position, smooth_offset(face, a, sa, b, sb));
            open[sa + 1][sb + 1] = !voxel || !solidity[voxel - 1];
        }
        const byte sealed = smooth_seal_light(light, open);
        for (byte v = 0; v < 4; v++) {
            vertex_lights[v] = sealed;
        }
        return;
    }
    for (byte i = 0; i < 2; i++) {
        const sbyte sign = i ? 1 : -1;
        solid_a[i] = smooth_grid_solid(grid, solidity, position, smooth_offset(face, a, sign, b, 0));
        solid_b[i] = smooth_grid_solid(grid, solidity, position, smooth_offset(face, a, 0, b, sign));
    }
    for (byte v = 0; v < 4; v++) {
        const sbyte sa = smooth_vertex_signs[v][0], sb = smooth_vertex_signs[v][1];
        vertex_lights[v] = smooth_vertex_light(light[1][1], light[sa + 1][1], light[1][sb + 1], light[sa + 1][sb + 1],
            solid_a[sa > 0], solid_b[sb > 0]);
    }
}

static inline void zox_apply_smooth_lights(
    const smooth_grid* grid,
    spinlock** lights_locks,
    spinlock** voxels_locks,
    const LightNode** lights,
    const VoxelNode** voxels,
    const byte* solidity,
    const VoxelNode* node,
    const SidesOctree* sides,
    const MeshColorRGBs* colors,
    byte3 position,
    uint* ccount,
    byte target_depth,
    byte depth)
{
    const SidesOctree* sides_kids =
        get_children_SidesOctree(sides);
    byte has_sides_kids = sides_kids != NULL;
    spinlock* node_lock = voxels_locks[OCTREE_INDEX(0, 0, 0)];
    // Dig Deeper
    if (depth < target_depth && has_sides_kids) {
        spin_lock(node_lock);
        const VoxelNode* node_kids = has_children_VoxelNode(node) ?
            get_children_VoxelNode(node) :
            NULL;
        spin_unlock(node_lock);
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            byte3 child_position = byte3_add(
                position,
                octree_positions[i]);
            zox_apply_smooth_lights(
                grid,
                lights_locks,
                voxels_locks,
                lights,
                voxels,
                solidity,
                node_kids ? &node_kids[i] : node,
                &sides_kids[i],
                colors,
                child_position,
                ccount,
                target_depth,
                depth);
//#ifdef zox_safety_checks
            if (*ccount + voxel_face_vertices_length > colors->length) {
                break;
            }
// #endif
        }
        return;
    }
    // NOTE: If no sides were rendered
    byte sides_visible = sides->value;
    if (!sides_visible) {
        return;
    }
    // the mesh builder skips air with sides (stale sides), so we do too
    spin_lock(node_lock);
    const byte is_air = !node->value;
    spin_unlock(node_lock);
    if (is_air) {
        return;
    }
    // each face the sides say is visible
    for (byte direction = 0; direction < 6; direction++) {
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
        if (*ccount + voxel_face_vertices_length > colors->length) {
            // more faces than the mesh has: it's older than the sides (build_smooth_lights_system rebuilds it)
            *ccount = colors->length + 1;
            return;
        }
        byte vertex_lights[4];
        // a face above the mesh's depth (a closed node) reads the trees: the grid is at the mesh's depth
        if (grid && depth == target_depth) {
            smooth_face_lights_grid(grid, solidity, position, direction, vertex_lights);
        } else {
            smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, position, depth, direction, vertex_lights);
        }
        for (byte v = 0; v < voxel_face_vertices_length; v++) {
            colors->value[(*ccount)++] = (color_rgb) { vertex_lights[v], vertex_lights[v], vertex_lights[v] };
        }
    }
}

// TODO: Add spinlock for SidesOctreeLock
// NOTE: Rebuilds Lights only when BuildMeshColors is dirty
void build_smooth_lights_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    if (!zox_smooth_lighting) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    // which blocks stop light, rebuilt when the block manager (realm) changes (fun/solidity.c)
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    entity realm = 0;
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshColorRGBs, colors);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        // Get chunk data: the mesh's material holds its sides
        entity material = zox_get_parent(world, e);
        entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Parent Invalid for [%s]", zox_getn(e));
            continue;
        }
#endif
        byte chunk_depth = zox_getv(chunk, NodeDepth);
        if (depth->value > chunk_depth) {
            if (dbg_log) {
                zox_log("Chunk Depth is wrong for Light Build [%s]",
                    zox_getn(chunk));
            }
            // can't light it, don't block its geometry upload
            zox_remove(e, BuildMeshColors);
            continue;
        }
        // lit already: relit once the lights round it settle (it keeps its colours till then)
        if (zox_has(e, MeshColorsBuilt) && zox_chunk_lights_settling(world, chunk)) {
            continue;
        }
        // If chunk building we wait
        if (zox_has(chunk, BuildChunkSides)) {
            if (dbg_log) {
                zox_log("Chunk [%s] is still Building Sides",
                    zox_getn(chunk));
            }
            continue;
        }
        const SidesOctree* sides = zox_get(material, SidesOctree);
        SidesOctreeLock* sides_lock = zox_mut(material, SidesOctreeLock);
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        // const VoxelNode* root_voxels = zox_get(chunk, VoxelNode);
        entity nearby_chunks[27];
        const LightNode* lights[27];
        const VoxelNode* voxels[27];
        spinlock* lights_locks[27];
        spinlock* voxels_locks[27];
        fetch_nearby_chunks(
            world,
            chunk,
            neighbors->value,
            nearby_chunks);
        fetch_nearby_lights(
            world,
            nearby_chunks,
            lights,
            lights_locks);
        fetch_nearby_voxels(
            world,
            nearby_chunks,
            voxels,
            voxels_locks);
        update_light_solidity(world, zox_get_link(world, chunk, BlockManagerLink), &realm, solidity);
        uint ccount = 0;
        smooth_grid grid = { 0 };
        const byte has_grid = smooth_grid_fill(&grid, lights, lights_locks, voxels, voxels_locks, depth->value);
        spin_lock(&sides_lock->value);
        zox_apply_smooth_lights(
            has_grid ? &grid : NULL,
            lights_locks,
            voxels_locks,
            lights,
            voxels,
            solidity,
            voxels[OCTREE_INDEX(0, 0, 0)],
            sides,
            colors,
            byte3_zero,
            &ccount,
            depth->value,
            0);
        spin_unlock(&sides_lock->value);
        free(grid.lights);
        zox_remove(e, BuildMeshColors);
        if (ccount > colors->length) {
            // stale: rebuilt (it lights again after), the sides changed since
            zox_smooth_lights_stale++;
            zox_logw("[smooth lights] mesh [%s] older than its chunk's sides, rebuilding it", zox_getn(e));
            zox_add(e, BuildMesh);
            continue;
        }
        zox_add(e, MeshColorsDirty);
        /*if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }*/
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(build_smooth_lights_system);
