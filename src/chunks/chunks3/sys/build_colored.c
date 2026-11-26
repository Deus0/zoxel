typedef struct {
    int_array_d* indicies;
    float3_array_d* vertices;
    color_array_d* colors;
} mesh_colored_build_data;

// a face at a voxel position (in voxels) of a mesh scale big, moved by offset (its bounds): its colours added after
void add_voxel_face(
    mesh_colored_build_data* mesh,
    float3 position,
    float3 offset,
    float scale,
    const int* face_indicies,
    const float3* face_verts)
{
    const float3 origin = { position.x * scale + offset.x, position.y * scale + offset.y, position.z * scale + offset.z };
    mesh_add_face(mesh->indicies, mesh->vertices, face_indicies, face_verts, origin, scale);
}

// the voxel beside a position hides its face: materials (a voxel value's kind, zox_block_material_*, NULL all solid)
//  and material (the mesh's) by block_material_hides; past the tree, its neighbours', none: edge
byte is_adjacent_all_solid(
    const byte* materials,
    byte material,
    byte edge,
    const VoxelNode **neighbors,
    const VoxelNode *node,
    int3 position,
    byte direction,
    byte depth)
{
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode(
        neighbors,
        node,
        position,
        depth,
        direction);
    if (!adjacent_node) {
        return edge;
    }
    byte reversed = reverse_direction(direction);
    byte axis = reversed >> 1;  // 0=x, 1=y, 2=z
    byte side = reversed & 1;   // 0=negative side, 1=positive side
    // zox_log("axis [%i] side [%i] from direction [%i]", axis, side, reversed);
    // we need full distance here
    return get_node_sides_all_hide(
        materials,
        material,
        adjacent_node,
        axis,
        side,
        8); // depth);
}

#define zox_vox_grid_max_depth 7

typedef struct {
    byte max_depth;
    const VoxelNode** levels[zox_vox_grid_max_depth + 1];
} vox_pyramid;

static inline int vox_grid_index(int length, int x, int y, int z) {
    return ((x + 1) * (length + 2) + (y + 1)) * (length + 2) + (z + 1);
}

static inline const VoxelNode* vox_pyramid_get(const vox_pyramid* pyramid, byte depth, int x, int y, int z) {
    return pyramid->levels[depth][vox_grid_index(octree_size(depth), x, y, z)];
}

static void fill_vox_pyramid(vox_pyramid* pyramid, const VoxelNode* node, byte depth, int3 position) {
    if (!node) {
        return;
    }
    pyramid->levels[depth][vox_grid_index(octree_size(depth), position.x, position.y, position.z)] = node;
    if (depth == pyramid->max_depth) {
        return;
    }
    if (has_children_VoxelNode(node)) {
        const VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            const int3 child_position = int3_add(int3_multiply_int(position, 2), byte3_to_int3(octree_positions[i]));
            fill_vox_pyramid(pyramid, &kids[i], depth + 1, child_position);
        }
        return;
    }
    // air below reads as no node, the same to faces and ao
    if (!node->value) {
        return;
    }
    // a leaf: it's the node all the way down
    for (byte level = depth + 1; level <= pyramid->max_depth; level++) {
        const int size = octree_size(level - depth);
        const int length = octree_size(level);
        const int3 start = int3_multiply_int(position, size);
        const VoxelNode** grid = pyramid->levels[level];
        for (int x = start.x; x < start.x + size; x++) {
            for (int y = start.y; y < start.y + size; y++) {
                for (int z = start.z; z < start.z + size; z++) {
                    grid[vox_grid_index(length, x, y, z)] = node;
                }
            }
        }
    }
}

// like is_adjacent_all_solid with no neighbor chunks
// where the colored walk finds a voxel's neighbours: the pyramid (a model up to zox_vox_grid_max_depth, an array
//  read, 4 to 5x the tree's speed: tst perf_meshing) or the octree (deeper); past the chunk's border, its neighbour
//  chunks' trees (noctrees, NULL for none) either way
typedef struct {
    const vox_pyramid* pyramid;     // NULL: read the octree
    const VoxelNode* root;
    const VoxelNode** noctrees;
    byte neighbors;                 // any neighbour chunk: the pyramid's border (air) is read through them
    const byte* materials;          // a voxel value's kind (zox_block_material_*), NULL: all solid, one mesh
    byte material;                  // the kind this mesh draws (solid, or a see through one)
} vox_lookup;

static inline byte vox_lookup_inside(byte depth, int3 p) {
    const int length = octree_size(depth);
    return p.x >= 0 && p.y >= 0 && p.z >= 0 && p.x < length && p.y < length && p.z < length;
}

// the voxel beside p hides the face it touches (all of it solid there: get_node_sides_all_hide); the pyramid's
//  border is air, so past it a neighbour chunk is read from its tree
static inline byte vox_lookup_adjacent_solid(const vox_lookup* lookup, byte depth, int3 p, byte direction) {
    if (!lookup->pyramid) {
        return is_adjacent_all_solid(lookup->materials, lookup->material, 0, lookup->noctrees, lookup->root, p, direction, depth);
    }
    const int3 adjacent = move_position(p, direction);
    if (lookup->neighbors && !vox_lookup_inside(depth, adjacent)) {
        return is_adjacent_all_solid(lookup->materials, lookup->material, 0, lookup->noctrees, lookup->root, p, direction, depth);
    }
    const VoxelNode* node = vox_pyramid_get(lookup->pyramid, depth, adjacent.x, adjacent.y, adjacent.z);
    if (!node) {
        return 0;
    }
    const byte reversed = reverse_direction(direction);
    return get_node_sides_all_hide(lookup->materials, lookup->material, node, reversed >> 1, reversed & 1, 8);
}

// a voxel at p is solid, for ambient occlusion: past the chunk's border it's air, see through ones don't count
static inline byte vox_lookup_solid(const vox_lookup* lookup, byte depth, int3 p) {
    if (!vox_lookup_inside(depth, p)) {
        return 0;
    }
    const VoxelNode* node = lookup->pyramid ? vox_pyramid_get(lookup->pyramid, depth, p.x, p.y, p.z) :
        get_VoxelNode(lookup->root, depth, int3_to_byte3(p));
    // see through voxels don't darken corners
    return node && node->value > 0 && (!lookup->materials || lookup->materials[node->value] == zox_block_material_solid);
}

// a voxel's faces from the pyramid alone (a model with no neighbour chunks, most of them): its border is air, every
//  neighbour an array read, the pointer kept local (read through the lookup each time it was 10% slower, release)
static inline void build_voxel_faces_pyramid(const vox_pyramid* pyramid, mesh_colored_build_data* mesh,
    color_rgb voxel_color, float scale, float3 bounds_offset, byte depth, byte3 position)
{
    const int3 p = byte3_to_int3(position);
    byte nsolids[6];
    byte hidden = 1;
    for (byte direction = 0; direction < 6; direction++) {
        const int3 adjacent = move_position(p, direction);
        const VoxelNode* node = vox_pyramid_get(pyramid, depth, adjacent.x, adjacent.y, adjacent.z);
        const byte reversed = reverse_direction(direction);
        nsolids[direction] = node && get_node_sides_all_hide(NULL, zox_block_material_solid, node, reversed >> 1, reversed & 1, 8);
        hidden &= nsolids[direction];
    }
    if (hidden) {
        return;
    }
    byte naos[27];
    byte i = 0;
    for (sbyte x = -1; x <= 1; x++) for (sbyte y = -1; y <= 1; y++) for (sbyte z = -1; z <= 1; z++) {
        const VoxelNode* node = vox_pyramid_get(pyramid, depth, p.x + x, p.y + y, p.z + z);
        naos[i++] = (x || y || z) && node && node->value > 0;
    }
    const float3 positionf = byte3_to_float3(position);
    for (byte direction = 0; direction < 6; direction++) {
        if (nsolids[direction]) {
            continue;
        }
        add_voxel_face(mesh, positionf, bounds_offset, scale, voxel_face_indicies_n + direction * voxel_face_indicies_length,
            voxel_face_vertices_n[direction]);
        if (zox_ambient_occlusion) {
            add_voxel_face_colors_ao_27(mesh->colors, voxel_color, direction, naos);
        } else {
            add_voxel_face_colors(mesh->colors, voxel_color, direction);
        }
    }
}

// a voxel's faces: each beside a voxel that doesn't hide it, its corners darkened by ambient occlusion
static void build_voxel_faces_colored(const vox_lookup* lookup, mesh_colored_build_data* mesh, color_rgb voxel_color,
    float scale, float3 bounds_offset, byte depth, byte3 position)
{
#ifdef zox_ambient_occlusion27
    if (lookup->pyramid && !lookup->neighbors && !lookup->materials) {
        build_voxel_faces_pyramid(lookup->pyramid, mesh, voxel_color, scale, bounds_offset, depth, position);
        return;
    }
#endif
    const int3 p = byte3_to_int3(position);
    byte nsolids[6];
    byte hidden = 1;
    for (byte direction = 0; direction < 6; direction++) {
        nsolids[direction] = vox_lookup_adjacent_solid(lookup, depth, p, direction);
        hidden &= nsolids[direction];
    }
    // hidden voxels add no faces, skip their ao lookups (most of a solid model)
    if (hidden) {
        return;
    }
#ifdef zox_ambient_occlusion27
    byte naos[27];
    byte i = 0;
    for (sbyte x = -1; x <= 1; x++) for (sbyte y = -1; y <= 1; y++) for (sbyte z = -1; z <= 1; z++) {
        naos[i++] = (x || y || z) && vox_lookup_solid(lookup, depth, (int3) { p.x + x, p.y + y, p.z + z });
    }
#else
    byte naos[6];
    for (byte direction = 0; direction < 6; direction++) {
        const byte asolid = is_adjacent_all_solid(lookup->materials, lookup->material, 255, lookup->noctrees, lookup->root, p, direction, depth);
        naos[direction] = asolid > 0 && asolid < 255;
    }
#endif
    const float3 positionf = byte3_to_float3(position);
    for (byte direction = 0; direction < 6; direction++) {
        if (nsolids[direction]) {
            continue;
        }
        add_voxel_face(mesh, positionf, bounds_offset, scale, voxel_face_indicies_n + direction * voxel_face_indicies_length,
            voxel_face_vertices_n[direction]);
        if (zox_ambient_occlusion) {
#ifdef zox_ambient_occlusion27
            add_voxel_face_colors_ao_27(mesh->colors, voxel_color, direction, naos);
#else
            add_voxel_face_colors_ao6(mesh->colors, voxel_color, direction, naos);
#endif
        } else {
            add_voxel_face_colors(mesh->colors, voxel_color, direction);
        }
    }
}

// the one colored walk: down the octree to the target depth, each voxel's faces (a uniform node cut into its voxels at
//  the target depth; greedy meshing merges them after, fun/greedy.c)
static void build_voxel_mesh_dig(
    const vox_lookup* lookup,
    const VoxelNode* voxels,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte target_depth,
    byte depth,
    byte3 position,
    float3 bounds_offset,
    float scale)
{
    if (!voxels) {
        return;
    }
    const byte has_vkids = has_children_VoxelNode(voxels);
    const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voxels): NULL;
    if (!has_vkids && !voxels->value) {
        return;
    }
    if (depth < target_depth) {
        depth++;
        scale *= 0.5f;
        byte3_multiply_byte(&position, 2);
        for (byte i = 0; i < octree_length; i++) {
            const VoxelNode* child_voxels = has_vkids ? &vkids[i] : voxels;
            build_voxel_mesh_dig(lookup, child_voxels, vcolors, mesh, target_depth, depth,
                byte3_add(position, octree_positions[i]), bounds_offset, scale);
        }
        return;
    }
    if (!voxels->value) {
        return;
    }
    if (lookup->materials && lookup->materials[voxels->value] != lookup->material) {
        return;     // another mesh's (another kind)
    }
    const byte voxel = voxels->value - 1;
    if (voxel >= vcolors->length) {
        zox_logw("Voxel Index OOB: %i >= %i", voxel, vcolors->length);
        return;
    }
    build_voxel_faces_colored(lookup, mesh, vcolors->value[voxel], scale, bounds_offset, depth, position);
}

// one kind's colored mesh of root at target_depth (materials: a voxel value's kind, zox_block_material_*, NULL all
//  solid; material: the kind it draws): its voxels' faces, hidden as terrain's are (block_material_hides: a solid hides
//  any face, a see through voxel only its own kind's), so a solid core shows through a see through shell (a slime's).
//  From the pyramid when pyramid is set and the model fits it, else the octree; noctrees its neighbour chunks
void build_voxel_mesh_material(
    const VoxelNode* root,
    const VoxelNode** noctrees,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte node_depth,
    byte target_depth,
    float3 bounds_offset,
    float scale,
    byte pyramid,
    const byte* materials,
    byte material)
{
    if (!root) {
        return;
    }
    const VoxelNode* no_neighbors[6] = { 0 };
    vox_lookup lookup = { .root = root, .noctrees = noctrees ? noctrees : no_neighbors, .materials = materials,
        .material = material };
    for (byte j = 0; j < 6; j++) {
        lookup.neighbors |= lookup.noctrees[j] != NULL;
    }
    vox_pyramid grid = { .max_depth = target_depth };
#ifndef zox_ambient_occlusion27
    pyramid = 0;
#endif
    pyramid = pyramid && target_depth <= zox_vox_grid_max_depth && target_depth <= node_depth;
    if (pyramid) {
        for (byte depth = 0; depth <= target_depth; depth++) {
            const int glength = octree_size(depth) + 2;
            grid.levels[depth] = calloc(glength * glength * glength, sizeof(VoxelNode*));
        }
        fill_vox_pyramid(&grid, root, 0, int3_zero);
        lookup.pyramid = &grid;
    }
    build_voxel_mesh_dig(&lookup, root, vcolors, mesh, target_depth, 0, byte3_zero, bounds_offset, scale);
    if (pyramid) {
        for (byte depth = 0; depth <= target_depth; depth++) {
            free((void*) grid.levels[depth]);
        }
    }
}

// a colored mesh of root at target_depth, every voxel solid
void build_voxel_mesh(
    const VoxelNode* root,
    const VoxelNode** noctrees,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte node_depth,
    byte target_depth,
    float3 bounds_offset,
    float scale,
    byte pyramid)
{
    build_voxel_mesh_material(root, noctrees, vcolors, mesh, node_depth, target_depth, bounds_offset, scale, pyramid,
        NULL, zox_block_material_solid);
}

// Builds Colored Vox Meshes
void chunk_colors_build_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(ColorRGBs);
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshColors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(NodeDepth, ndepth);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(ColorRGBs, vcolors);
        zox_sys_i(ChunkSize, csize);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, vertices);
        zox_sys_o(MeshColors, colors);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        // a chunk of a colored chunk3 parent takes its parent's palette and material ids (spawn_chunk3_parent_colored)
        const entity owner = vcolors->length ? e : zox_get_parent(world, e);
        const ColorRGBs* palette = vcolors->length ? vcolors :
            (zox_valid(owner) && zox_has(owner, ColorRGBs) ? zox_get(owner, ColorRGBs) : vcolors);
        if (!palette->length) {
            continue;
        }
        clear_mesh(indicies, vertices, colors);
        const float voxel_size = scale->value * octree_size(ndepth->value) / octree_size(rdepth->value);
        // its neighbours' trees, faces into them hidden where they're solid
        const VoxelNode* noctrees[6];
        for (byte j = 0; j < 6; j++) {
            entity n = neighbors->value[j];
            noctrees[j] = n ? zox_get(neighbors->value[j], VoxelNode) : NULL;
        }
        ushort vlength = octree_size(ndepth->value);
        float3 bounds = calculate_vox_bounds(
            csize->value, // chunk_size,
            scale->value);
        float3 position = float3_scale(bounds, -1);
        float cscale = scale->value * vlength;
        /*zox_log("Colored Chunk [%s] Size [%ix%ix%i] Offset [%fx%fx%f] - Scale [%f]",
            zox_sys_e_name,
            csize->value.x,
            csize->value.y,
            csize->value.z,
            position.x,
            position.y,
            position.z,
            scale->value);*/
        // initialize our mesh data
        mesh_colored_build_data mesh = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .colors = create_color_array_d(initial_dynamic_array_size)
        };
        // its octree and neighbors' locked while walked (thread_safety.md)
        spinlock* neighbor_locks[6];
        zox_octree_neighbor_locks(world, neighbors, neighbor_locks);
        spinlock* locked[7];
        const byte locked_count = zox_lock_voxel_trees(zox_octree_lock(world, e), neighbor_locks, locked);
        // its solid voxels (all of them without MaterialIDs)
        byte materials[256];
        const byte other = vox_material_table(zox_has(owner, MaterialIDs) ? zox_get(owner, MaterialIDs) : NULL, materials);
        build_voxel_mesh_material(voxels, noctrees, palette, &mesh, ndepth->value, rdepth->value, position, cscale, 1, other ? materials : NULL, zox_block_material_solid);
        // its see through voxels into its see through child (set_chunk3_parent_colored_materials), with its neighbours
        if (other) {
            entity shell = 0;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !shell; j++) {
                    shell = zox_has(it2.entities[j], TransparentMesh) ? it2.entities[j] : 0;
                }
            }
            if (zox_valid(shell)) {
                mesh_colored_build_data shell_mesh = {
                    .indicies = create_int_array_d(initial_dynamic_array_size),
                    .vertices = create_float3_array_d(initial_dynamic_array_size),
                    .colors = create_color_array_d(initial_dynamic_array_size)
                };
                build_voxel_mesh_material(voxels, noctrees, palette, &shell_mesh, ndepth->value, rdepth->value, position,
                    cscale, 1, materials, zox_block_material_liquid);
                MeshIndicies* shell_indicies = zox_get_mut(shell, MeshIndicies);
                MeshVertices* shell_vertices = zox_get_mut(shell, MeshVertices);
                MeshColors* shell_colors = zox_get_mut(shell, MeshColors);
                clear_mesh(shell_indicies, shell_vertices, shell_colors);
                shell_indicies->length = shell_mesh.indicies->size;
                shell_vertices->length = shell_mesh.vertices->size;
                shell_colors->length = shell_mesh.colors->size;
                shell_indicies->value = finalize_arrayd_int(shell_mesh.indicies);
                shell_vertices->value = finalize_arrayd_float3(shell_mesh.vertices);
                shell_colors->value = finalize_arrayd_color(shell_mesh.colors);
                if (zox_greedy_mesh) {
                    mesh_greedy(shell_indicies, shell_vertices, shell_colors, NULL, position, voxel_size);
                }
                zox_add(shell, MeshDirty);
            }
        }
        zox_unlock_voxel_trees(locked, locked_count);
        indicies->length = mesh.indicies->size;
        vertices->length = mesh.vertices->size;
        colors->length = mesh.colors->size;
        indicies->value = finalize_arrayd_int(mesh.indicies);
        vertices->value = finalize_arrayd_float3(mesh.vertices);
        colors->value = finalize_arrayd_color(mesh.colors);
        if (zox_greedy_mesh) {
            mesh_greedy(indicies, vertices, colors, NULL, position, voxel_size);
        }
        zox_remove(e, BuildMesh);
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("[%s] has built a colored mesh: Tris [%i]", zox_getn(e), indicies->length / 3);
        }
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(chunk_colors_build_system);
