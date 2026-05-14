byte is_adjacent_all_solid(const byte* solidity, byte edge, const VoxelNode **neighbors, const VoxelNode *node, int3 position, byte direction, byte depth) {
    const VoxelNode* anode = get_adjacentn_VoxelNode(neighbors, node, position, depth, direction);
    byte rdirection = reverse_direction(direction);
    return anode ?
    get_node_sides_all_solid(solidity, anode, rdirection, depth) :
    edge;
}

void build_voxel_faces_c(const VoxelNode* root, const VoxelNode** noctrees, mesh_colored_build_data* mesh, color_rgb voxel_color, float scale, float3 positionf, float3 bounds_offset, byte depth, byte3 position) {
    // this is for rendering neighbor face
    byte edge = 0;
    byte nsolids[6];
    for (byte direction = 0; direction < 6; direction++) {
        nsolids[direction] = is_adjacent_all_solid(NULL, edge, noctrees, root, byte3_to_int3(position), direction, depth);

    }
#ifdef zox_ambient_occlusion27
    byte vlength = powers_of_two[depth];
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
                } else if (nposition.x < 0 || nposition.x >= vlength ||
                    nposition.y < 0 || nposition.y >= vlength ||
                    nposition.z < 0 || nposition.z >= vlength) {
                    naos[i] = 0;
                } else {
                    const VoxelNode* noctree = get_VoxelNode(root, depth, int3_to_byte3(nposition), 0);
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

void build_voxel_mesh_c(const VoxelNode* root, const VoxelNode* voctree, const VoxelNode** noctrees, const byte* nrdepths, const ColorRGBs* vcolors, mesh_colored_build_data* mesh, byte node_depth, byte depth, byte3 position, float3 bounds_offset, float scale) {
    // If data is null
    if (!voctree) {
        return;
    }
    // Dig Deeper
    if (depth < node_depth && !is_closed_VoxelNode(voctree)) {
        depth++;
        scale *= 0.5f;
        byte3_multiply_byte(&position, 2);
        VoxelNode* kids = get_children_VoxelNode(voctree);
        for (byte i = 0; i < octree_length; i++) {
            // Models dont have these set??
            if (!kids[i].value) {
                continue;
            }
            byte3 cposition = byte3_add(position, octree_positions_b[i]);
            build_voxel_mesh_c(root, &kids[i], noctrees, nrdepths, vcolors, mesh, node_depth, depth, cposition, bounds_offset, scale);
        }
        return;
    }
    // If Air we return
    if (!voctree->value) {
        return;
    }
    // final
    byte voxel = voctree->value - 1;
    if (voxel >= vcolors->length) {
        zox_logw("Voxel Index OOB: %i >= %i", voxel, vcolors->length);
        return;
    }
    color_rgb voxel_color = vcolors->value[voxel];
    float3 positionf = byte3_to_float3(position);
    build_voxel_faces_c(root, noctrees, mesh, voxel_color, scale, positionf, bounds_offset, depth, position);
}

// Builds Colored Vox Meshes
zox_sys2(ChunkColorsBuildSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkMeshDirty);
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
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, dirty);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(NodeDepth, ndepth);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(ColorRGBs, vcolors);
        zox_sys_i(ChunkSize, csize);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(MeshDirty, mdirty);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, vertices);
        zox_sys_o(MeshColorRGBs, colors);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!vcolors->length) {
            // zox_sys_e();
            // zox_logw("Vox has no colors [%s:%lu]", zox_get_name(e), e);
            continue;
        }
        // removes mesh when 255
        clear_mesh(indicies, vertices, colors);
        if (rdepth->value >= render_depth_spawning) {
            mdirty->value = mesh_state_trigger_slow;
            continue;
        }
        // fetch neighbor depths and nodes
        const VoxelNode* noctrees[6];
        byte nrdepths[6];
        for (byte j = 0; j < 6; j++) {
            entity n = neighbors->value[j];
            noctrees[j] = n ? zox_gett(neighbors->value[j], VoxelNode) : NULL;
            nrdepths[j] = n ? zox_gett_value(n, RenderDepth) : 0;
        }
        float3 b = calculate_vox_bounds(csize->value, scale->value);
        float3 position = float3_scale(b, -1);
        byte vlength = powers_of_two[ndepth->value];
        float cscale = scale->value * vlength;
        // initialize our mesh data
        mesh_colored_build_data mesh = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .colors = create_color_rgb_array_d(initial_dynamic_array_size)
        };
        read_lock_VoxelNode(voctree);
        build_voxel_mesh_c(voctree, voctree, noctrees, nrdepths, vcolors, &mesh, rdepth->value, 0, byte3_zero, position, cscale);
        read_unlock_VoxelNode(voctree);
        clear_mesh(indicies, vertices, colors);
        indicies->length = mesh.indicies->size;
        vertices->length = mesh.vertices->size;
        colors->length = mesh.colors->size;
        indicies->value = zinalize_int_array_d(mesh.indicies);
        vertices->value = zinalize_float3_array_d(mesh.vertices);
        colors->value = zinalize_color_rgb_array_d(mesh.colors);
        mdirty->value = mesh_state_trigger_slow;
    }
} zox_sys_end(ChunkColorsBuildSystem);
