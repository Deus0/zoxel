// NOTE: Scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void add_voxel_face(mesh_colored_build_data* mesh, float3 position, float3 offset, float scale, const int* face_indicies, const float3* face_verts) {
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

byte is_adjacent_all_solid(const byte* solidity, byte edge, const VoxelNode **neighbors, const VoxelNode *node, int3 position, byte direction, byte depth) {
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode(neighbors, node, position, depth, direction);
    if (!adjacent_node) {
        return edge;
    }
    byte reversed = reverse_direction(direction);
    byte axis = reversed >> 1;  // 0=x, 1=y, 2=z
    byte side = reversed & 1;   // 0=negative side, 1=positive side
    // zox_log("axis [%i] side [%i] from direction [%i]", axis, side, reversed);
    // we need full distance here
    return get_node_sides_all_solid(solidity, adjacent_node, axis, side, 8); // depth);
}

void build_voxel_faces_colored(const VoxelNode* root, const VoxelNode** noctrees, mesh_colored_build_data* mesh, color_rgb voxel_color, float scale, float3 positionf, float3 bounds_offset, byte depth, byte3 position) {
    if (!root) {
        return;
    }
    // this is for rendering neighbor face
    byte edge = 0;
    byte nsolids[6];
    for (byte direction = 0; direction < 6; direction++) {
        nsolids[direction] = is_adjacent_all_solid(NULL, edge, noctrees, root, byte3_to_int3(position), direction, depth);
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

void build_voxel_mesh_c(const VoxelNode* root, const VoxelNode* voxels, const VoxelNode** noctrees, const byte* nrdepths, const ColorRGBs* vcolors, mesh_colored_build_data* mesh, byte target_depth, byte depth, byte3 position, float3 bounds_offset, float scale, byte is_split) {
    // If data is null
    if (!voxels) {
        return;
    }
    // Dig Deeper
    byte has_vkids = !is_closed_VoxelNode(voxels);
    const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voxels) : NULL;
    byte is_dig = depth < target_depth;
    if (!is_split) { // || !zox_split_colored_chunks) {
        is_dig &= has_vkids;
    }
    if (is_dig) { // && !is_closed_VoxelNode(voxels)) {
        depth++;
        scale *= 0.5f;
        byte3_multiply_byte(&position, 2);
        // VoxelNode* kids = get_children_VoxelNode(voxels);
        for (byte i = 0; i < octree_length; i++) {
            // Models dont have these set??
            const VoxelNode* child_voxels = has_vkids ? &vkids[i] : voxels;
            /*if (!kids[i].value) {
                continue;
            }*/
            byte3 child_position = byte3_add(position, octree_positions_b[i]);
            build_voxel_mesh_c(root, child_voxels, noctrees, nrdepths, vcolors, mesh, target_depth, depth, child_position, bounds_offset, scale, is_split);
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
    build_voxel_faces_colored(root, noctrees, mesh, voxel_color, scale, positionf, bounds_offset, depth, position);
}

// Builds Colored Vox Meshes
zox_sys2(ChunkColorsBuildSystem) {
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
    zox_sys_out(BuildMesh);
    zox_sys_out(MeshDirty);
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
        zox_sys_o(BuildMesh, build);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (build->value == zox_dirty_trigger) {
            build->value = zox_dirty_active;
            continue;
        }
        if (build->value == zox_dirty_active) {
            build->value = zox_dirty_end;
            continue;
        }
        if (build->value != zox_dirty_end) {
            continue;
        }
        if (!vcolors->length) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        clear_mesh(indicies, vertices, colors);
        byte is_split = !zox_has(e, NoSplitChunk);
        // fetch neighbor depths and nodes
        const VoxelNode* noctrees[6];
        byte nrdepths[6];
        for (byte j = 0; j < 6; j++) {
            entity n = neighbors->value[j];
            noctrees[j] = n ? zox_gett(neighbors->value[j], VoxelNode) : NULL;
            nrdepths[j] = n ? zox_getv(n, RenderDepth) : 0;
        }
        float3 b = calculate_vox_bounds(csize->value, scale->value);
        float3 position = float3_scale(b, -1);
        short vlength = octree_size(ndepth->value);
        float cscale = scale->value * vlength;
        // initialize our mesh data
        mesh_colored_build_data mesh = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .colors = create_color_rgb_array_d(initial_dynamic_array_size)
        };
        read_lock_VoxelNode(voxels);
        build_voxel_mesh_c(voxels, voxels, noctrees, nrdepths, vcolors, &mesh, rdepth->value, 0, byte3_zero, position, cscale, is_split);
        read_unlock_VoxelNode(voxels);
        indicies->length = mesh.indicies->size;
        vertices->length = mesh.vertices->size;
        colors->length = mesh.colors->size;
        indicies->value = finalize_arrayd_int(mesh.indicies);
        vertices->value = finalize_arrayd_float3(mesh.vertices);
        colors->value = finalize_arrayd_color_rgb(mesh.colors);
        build->value = 0;
        mesh_dirty->value = zox_has(e, Skeleton) ? mesh_state_skeleton_trigger : mesh_state_trigger;
        if (dbg_log) {
            zox_log("[%s] has built a colored mesh: Tris [%i]", zox_getn(e), indicies->length / 3);
        }
        zox_sys_increment();
    }
} zox_sys_end(ChunkColorsBuildSystem);
