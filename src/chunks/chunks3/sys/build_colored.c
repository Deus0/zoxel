typedef struct {
    int_array_d* indicies;
    float3_array_d* vertices;
    color_rgb_array_d* colors;
} mesh_colored_build_data;

// NOTE: Scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void add_voxel_face(
    mesh_colored_build_data* mesh,
    float3 position,
    float3 offset,
    float scale,
    const int* face_indicies,
    const float3* face_verts)
{
    for (byte i = 0; i < 6; i++) {
        int index = mesh->vertices->size + face_indicies[i];
        int_array_d_add(mesh->indicies, index);
    }
    for (byte i = 0; i < 4; i++) {
        float3 vertex = face_verts[i];
        float3_add_float3_p(&vertex, position);
        float3_scale_p(&vertex, scale);
        float3_add_float3_p(&vertex, offset);
        float3_array_d_add(mesh->vertices, vertex);
        // color_rgb_array_d_add(mesh->colors, color_rgb_white);
    }
}

byte is_adjacent_all_solid(
    const byte* solidity,
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
    return get_node_sides_all_solid(
        solidity,
        adjacent_node,
        axis,
        side,
        8); // depth);
}

void build_voxel_faces_colored(
    const VoxelNode* root,
    const VoxelNode** noctrees,
    mesh_colored_build_data* mesh,
    color_rgb voxel_color,
    float scale,
    float3 positionf,
    float3 bounds_offset,
    byte depth,
    byte3 position)
{
    if (!root) {
        return;
    }
    // this is for rendering neighbor face
    byte edge = 0;
    byte nsolids[6];
    for (byte direction = 0; direction < 6; direction++) {
        nsolids[direction] = is_adjacent_all_solid(NULL, edge, noctrees, root, byte3_to_int3(position), direction, depth);
    }
    // hidden voxels add no faces, skip their ao lookups (most of a solid model)
    if (nsolids[0] && nsolids[1] && nsolids[2] && nsolids[3] && nsolids[4] && nsolids[5]) {
        return;
    }
    // bake our27 neighbor voxels
#ifdef zox_ambient_occlusion27
    short vlength = octree_size(depth);
    byte naos[27];
    byte i = 0;
    int3 nposition = byte3_to_int3(position);
    for (sbyte x = -1; x <= 1; x++) {
        nposition.x = position.x + x;
        for (sbyte y = -1; y <= 1; y++) {
            nposition.y = position.y + y;
            for (sbyte z = -1; z <= 1; z++) {
                nposition.z = position.z + z;
                if (x == 0 && y == 0 && z == 0) {
                    naos[i] = 0;
                } else if (nposition.x < 0 || nposition.x >= vlength || nposition.y < 0 || nposition.y >= vlength || nposition.z < 0 || nposition.z >= vlength) {
                    naos[i] = 0;
                } else {
                    const VoxelNode* noctree = get_VoxelNode(root, depth, int3_to_byte3(nposition));
                    naos[i] = noctree ? noctree->value > 0 : edge;
                }
                i++;
            }
        }
    }
#else
    byte naos[6];
    for (byte direction = 0; direction < 6; direction++) {
        byte asolid = is_adjacent_all_solid(NULL, 255, noctrees, root, byte3_to_int3(position), direction, depth);
        naos[direction] = asolid > 0 && asolid < 255;
    }
#endif
    for (byte direction = 0; direction < 6; direction++) {
        if (nsolids[direction]) {
            continue;
        }
        const int* indiciesf = voxel_face_indicies_n + direction * voxel_face_indicies_length;
        const float3* verticesf = voxel_face_vertices_v2[direction];
        add_voxel_face(mesh, positionf, bounds_offset, scale, indiciesf, verticesf);
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

void build_voxel_mesh_c(
    const VoxelNode* root,
    const VoxelNode* voxels,
    const VoxelNode** noctrees,
    const byte* nrdepths,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte target_depth,
    byte depth,
    byte3 position,
    float3 bounds_offset,
    float scale,
    byte is_split)
{
    // If data is null
    if (!voxels) {
        return;
    }
    // Dig Deeper
    byte has_vkids = has_children_VoxelNode(voxels);
    const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voxels): NULL;
    // an air leaf is air all the way down, no faces (split digs into leaves, a depth 7 vox is 2M cells)
    if (!has_vkids && !voxels->value) {
        return;
    }
    byte is_dig = depth < target_depth;
    if (!is_split) { // || !zox_split_colored_chunks) {
        is_dig &= has_vkids;
    }
    if (is_dig) { // && !is_closed_VoxelNode(voxels)) {
        depth++;
        scale *= 0.5f;
        byte3_multiply_byte(&position, 2);
        for (byte i = 0; i < octree_length; i++) {
            // Models dont have these set??
            const VoxelNode* child_voxels = has_vkids ? &vkids[i] : voxels;
            byte3 child_position = byte3_add(position, octree_positions[i]);
            build_voxel_mesh_c(
                root,
                child_voxels,
                noctrees,
                nrdepths,
                vcolors,
                mesh,
                target_depth,
                depth,
                child_position,
                bounds_offset,
                scale,
                is_split);
        }
        return;
    }
    // If Air we return
    if (!voxels->value) {
        return;
    }
    // final
    byte voxel = voxels->value - 1;
    if (voxel >= vcolors->length) {
        zox_logw("Voxel Index OOB: %i >= %i", voxel, vcolors->length);
        return;
    }
    color_rgb voxel_color = vcolors->value[voxel];
    float3 positionf = byte3_to_float3(position);
    build_voxel_faces_colored(
        root,
        noctrees,
        mesh,
        voxel_color,
        scale,
        positionf,
        bounds_offset,
        depth,
        position);
}

// Pyramid mesh building, for a vox without neighbors (models, skeletons)
//  - the octree flattened once into a padded grid of node pointers per depth (a pyramid), so neighbor lookups
//    are array reads, not lookups from the root (a depth 5 model: 29ms -> 5ms)
//  - each cell is what get_VoxelNode returns there: the node at that depth, or the leaf above it
//  - levels down to the target depth only, deeper side checks walk the octree, air leaves aren't filled down
//    (calloc'd pages nobody writes stay free), a sparse depth 7 character is cheap
//  - walks the octree like build_voxel_mesh_c (split or not, any lod), the same faces and ao in the same order
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
static inline byte vox_pyramid_adjacent_solid(const vox_pyramid* pyramid, byte depth, int3 position, byte direction) {
    const int3 adjacent = move_position(position, direction);
    const VoxelNode* node = vox_pyramid_get(pyramid, depth, adjacent.x, adjacent.y, adjacent.z);
    if (!node) {
        return 0;
    }
    const byte reversed = reverse_direction(direction);
    return get_node_sides_all_solid(NULL, node, reversed >> 1, reversed & 1, 8);
}

static void build_voxel_faces_pyramid(
    const vox_pyramid* pyramid,
    mesh_colored_build_data* mesh,
    color_rgb voxel_color,
    float scale,
    float3 bounds_offset,
    byte depth,
    byte3 position)
{
    const int3 p = byte3_to_int3(position);
    byte nsolids[6];
    byte hidden = 1;
    for (byte direction = 0; direction < 6; direction++) {
        nsolids[direction] = vox_pyramid_adjacent_solid(pyramid, depth, p, direction);
        hidden &= nsolids[direction];
    }
    // hidden voxels add no faces
    if (hidden) {
        return;
    }
    byte naos[27];
    byte i = 0;
    for (sbyte x = -1; x <= 1; x++) {
        for (sbyte y = -1; y <= 1; y++) {
            for (sbyte z = -1; z <= 1; z++) {
                if (!x && !y && !z) {
                    naos[i++] = 0;
                    continue;
                }
                const VoxelNode* node = vox_pyramid_get(pyramid, depth, p.x + x, p.y + y, p.z + z);
                naos[i++] = node && node->value > 0;
            }
        }
    }
    const float3 positionf = byte3_to_float3(position);
    for (byte direction = 0; direction < 6; direction++) {
        if (nsolids[direction]) {
            continue;
        }
        const int* indiciesf = voxel_face_indicies_n + direction * voxel_face_indicies_length;
        const float3* verticesf = voxel_face_vertices_v2[direction];
        add_voxel_face(mesh, positionf, bounds_offset, scale, indiciesf, verticesf);
#ifdef zox_ambient_occlusion27
        if (zox_ambient_occlusion) {
            add_voxel_face_colors_ao_27(mesh->colors, voxel_color, direction, naos);
        } else {
            add_voxel_face_colors(mesh->colors, voxel_color, direction);
        }
#endif
    }
}

// build_voxel_mesh_c's walk, with pyramid lookups
static void build_voxel_mesh_pyramid_dig(
    const vox_pyramid* pyramid,
    const VoxelNode* voxels,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte target_depth,
    byte depth,
    byte3 position,
    float3 bounds_offset,
    float scale,
    byte is_split)
{
    if (!voxels) {
        return;
    }
    const byte has_vkids = has_children_VoxelNode(voxels);
    const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voxels): NULL;
    if (!has_vkids && !voxels->value) {
        return;
    }
    byte is_dig = depth < target_depth;
    if (!is_split) {
        is_dig &= has_vkids;
    }
    if (is_dig) {
        depth++;
        scale *= 0.5f;
        byte3_multiply_byte(&position, 2);
        for (byte i = 0; i < octree_length; i++) {
            const VoxelNode* child_voxels = has_vkids ? &vkids[i] : voxels;
            build_voxel_mesh_pyramid_dig(
                pyramid,
                child_voxels,
                vcolors,
                mesh,
                target_depth,
                depth,
                byte3_add(position, octree_positions[i]),
                bounds_offset,
                scale,
                is_split);
        }
        return;
    }
    if (!voxels->value) {
        return;
    }
    const byte voxel = voxels->value - 1;
    if (voxel >= vcolors->length) {
        zox_logw("Voxel Index OOB: %i >= %i", voxel, vcolors->length);
        return;
    }
    build_voxel_faces_pyramid(pyramid, mesh, vcolors->value[voxel], scale, bounds_offset, depth, position);
}

// returns 0 if it couldn't (use build_voxel_mesh_c)
byte build_voxel_mesh_grid(
    const VoxelNode* root,
    const ColorRGBs* vcolors,
    mesh_colored_build_data* mesh,
    byte node_depth,
    byte target_depth,
    float3 bounds_offset,
    float scale,
    byte is_split)
{
#ifndef zox_ambient_occlusion27
    return 0;
#endif
    if (!root || target_depth > zox_vox_grid_max_depth || target_depth > node_depth) {
        return 0;
    }
    vox_pyramid pyramid = { .max_depth = target_depth };
    for (byte depth = 0; depth <= target_depth; depth++) {
        const int glength = octree_size(depth) + 2;
        pyramid.levels[depth] = calloc(glength * glength * glength, sizeof(VoxelNode*));
    }
    fill_vox_pyramid(&pyramid, root, 0, int3_zero);
    build_voxel_mesh_pyramid_dig(&pyramid, root, vcolors, mesh,
        target_depth, 0, byte3_zero, bounds_offset, scale, is_split);
    for (byte depth = 0; depth <= target_depth; depth++) {
        free((void*) pyramid.levels[depth]);
    }
    return 1;
}

// Builds Colored Vox Meshes
void ChunkColorsBuildSystem(iter* it) {
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
    zox_sys_out(MeshColorRGBs);
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
        zox_sys_o(MeshColorRGBs, colors);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        if (!vcolors->length) {
            continue;
        }
        clear_mesh(indicies, vertices, colors);
        byte is_split = !zox_has(e, NoSplitChunk);
        // fetch neighbor depths and nodes
        const VoxelNode* noctrees[6];
        byte nrdepths[6];
        for (byte j = 0; j < 6; j++) {
            entity n = neighbors->value[j];
            noctrees[j] = n ? zox_get(neighbors->value[j], VoxelNode) : NULL;
            nrdepths[j] = n ? zox_getv(n, RenderDepth) : 0;
        }
        ushort vlength = octree_size(ndepth->value);
        int3 chunk_size = int3_single(vlength);
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
            .colors = create_color_rgb_array_d(initial_dynamic_array_size)
        };
        // its octree and neighbors' locked while walked (thread_safety.md)
        spinlock* neighbor_locks[6];
        zox_octree_neighbor_locks(world, neighbors, neighbor_locks);
        spinlock* locked[7];
        const byte locked_count = zox_lock_voxel_trees(zox_octree_lock(world, e), neighbor_locks, locked);
        build_voxel_mesh_c(
            voxels,
            voxels,
            noctrees,
            nrdepths,
            vcolors,
            &mesh,
            rdepth->value,
            0,
            byte3_zero,
            position,
            cscale,
            is_split);
        zox_unlock_voxel_trees(locked, locked_count);
        indicies->length = mesh.indicies->size;
        vertices->length = mesh.vertices->size;
        colors->length = mesh.colors->size;
        indicies->value = finalize_arrayd_int(mesh.indicies);
        vertices->value = finalize_arrayd_float3(mesh.vertices);
        colors->value = finalize_arrayd_color_rgb(mesh.colors);
        zox_remove(e, BuildMesh);
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("[%s] has built a colored mesh: Tris [%i]", zox_getn(e), indicies->length / 3);
        }
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(ChunkColorsBuildSystem);
