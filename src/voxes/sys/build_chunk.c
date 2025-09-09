// todo: remove macros, they're not useful here, debugging and maintainance bad
// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset


void add_voxel_face_colors(
    color_rgb_array_d* color_rgbs,
    color_rgb voxel_color,
    const byte direction
) {
    for (byte a = 0; a < voxel_face_vertices_length; a++) {
        color_rgb vertex_color = voxel_color;

        if (direction == direction_down) {
            color_rgb_multiply_float(&vertex_color, 0.33f);
        } else if (direction == direction_front) {
            color_rgb_multiply_float(&vertex_color, 0.44f);
        } else if (direction == direction_left) {
            color_rgb_multiply_float(&vertex_color, 0.55f);
        } else if (direction == direction_back) {
            color_rgb_multiply_float(&vertex_color, 0.66f);
        } else if (direction == direction_right) {
            color_rgb_multiply_float(&vertex_color, 0.76f);
        }
        add_to_color_rgb_array_d(color_rgbs, vertex_color);
    }
}

void add_voxel_face(
    int_array_d *indicies,
    float3_array_d* vertices,
    const float3 offset,
    const float3 bounds_offset,
    const float scale,
    const int* indiciesf,
    const float3* verticesf
) {
    expand_capacity_int_array_d(indicies, voxel_face_indicies_length);
    for (int i = 0, j = indicies->size; i < voxel_face_indicies_length; i++, j++) {
        indicies->data[j] = vertices->size + indiciesf[i];
    }
    indicies->size += voxel_face_indicies_length;

    expand_capacity_float3_array_d(vertices, voxel_face_vertices_length);
    for (int i = 0, j = vertices->size; i < voxel_face_vertices_length; i++, j++) {
        vertices->data[j] = verticesf[i];
        float3_add_float3_p(&vertices->data[j], offset);
        float3_scale_p(&vertices->data[j], scale);
        float3_add_float3_p(&vertices->data[j], bounds_offset);
    }
    vertices->size += voxel_face_vertices_length;
}

void build_voxel_faces_c(
    const VoxelNode* root,
    const VoxelNode** cneighbors,
    // const byte* cnrender_depths,
    int_array_d* indicies,
    float3_array_d* vertices,
    color_rgb_array_d* color_rgbs,
    const color_rgb voxel_color,
    const float scale,
    const float3 offset,
    const float3 bounds_offset,
    byte depth,
    int3 position
) {
    const byte edge = 0; // 255
    byte neighbors[6];
    for (int i = 0; i < 6; i++) {
        neighbors[i] = is_adjacent_all_solid(
            NULL,
            edge,
            cneighbors,
            // cnrender_depths,
            root,
            position,
            i,          // direction
            depth
        );
    }

    for (int i = 0; i < 6; i++) {
        if (!neighbors[i] || neighbors[i] == 255) {
            const int* indiciesf = voxel_face_indicies_n + i * voxel_face_indicies_length;
            const float3* verticesf = voxel_face_vertices_n[i];

            add_voxel_face(
                indicies,
                vertices,
                offset,
                bounds_offset,
                scale,
                indiciesf,
                verticesf
            );

#ifdef DISABLE_AO
            add_voxel_face_colors(
                color_rgbs,
                voxel_color,
                i
            );
#else
            add_voxel_face_colors_ao(
                color_rgbs,
                voxel_color,
                i,
                neighbors
            );
#endif
        }
        /*if (i != 1 && i != 3) { // why does up need different indicies??
            is_positive = !is_positive;
        }*/
    }
}

void build_voxel_mesh_c(
    const VoxelNode* root,
    const VoxelNode* node,
    const VoxelNode** neighbors,
    const byte* nrender_depths,
    const ColorRGBs *colorRGBs,
    int_array_d *indicies,
    float3_array_d* vertices,
    color_rgb_array_d* color_rgbs,
    const byte node_depth,
    byte depth,
    int3 position,
    const float3 bounds_offset,
    float scale
) {
    if (node == NULL) {
        return;
    }
    if ((uintptr_t) node < 0x100) {
        zox_log_error("trash node detected.");
        return;
    }
    if (depth >= node_depth || is_closed_VoxelNode(node)) {
        const byte voxel = node->value;
        if (voxel) {
            const byte voxel_index = voxel - 1;
            if (voxel_index >= colorRGBs->length) {
                zox_logw("voxel_index oob: %i / %i", voxel_index, colorRGBs->length);
                return;
            }
            const color_rgb voxel_color = colorRGBs->value[voxel_index];
            float3 offset = float3_from_int3(position);

            build_voxel_faces_c(
                root,
                neighbors,
                // nrender_depths,
                indicies,
                vertices,
                color_rgbs,
                voxel_color,
                scale,
                offset,
                bounds_offset,
                depth,
                position
            );
        }
    } else {
        depth++;
        scale *= 0.5f;
        int3_multiply_int_p(&position, 2);
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            // Models dont have these set??
            if (!kids[i].value) {
                // continue;
            }
            int3 child_position = int3_add(position, octree_positions[i]);
            build_voxel_mesh_c(
                root,
                &kids[i],
                neighbors,
                nrender_depths,
                colorRGBs,
                indicies,
                vertices,
                color_rgbs,
                node_depth,
                depth,
                child_position,
                bounds_offset,
                scale);
        }
    }
}

void build_node_mesh_colors(
    const VoxelNode *node,
    const ColorRGBs *colorRGBs,
    MeshIndicies *meshIndicies,
    MeshVertices *meshVertices,
    MeshColorRGBs *meshColorRGBs,
    const byte chunk_depth,
    const VoxelNode** neighbors,
    const byte* nrender_depths,
    const float3 bounds_offset,
    const float scale
) {
    int_array_d* indicies = create_int_array_d(initial_dynamic_array_size);
    float3_array_d* vertices = create_float3_array_d(initial_dynamic_array_size);
    color_rgb_array_d* color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size);
    build_voxel_mesh_c(
        node,
        node,
        neighbors,
        nrender_depths,
        colorRGBs,
        indicies,
        vertices,
        color_rgbs,
        chunk_depth,
        0,
        int3_zero,
        bounds_offset,
        scale);
    clear_mesh(
        meshIndicies,
        meshVertices,
        meshColorRGBs);
    meshIndicies->length = indicies->size;
    meshVertices->length = vertices->size;
    meshColorRGBs->length = color_rgbs->size;
    meshIndicies->value = zinalize_int_array_d(indicies);
    meshVertices->value = zinalize_float3_array_d(vertices);
    meshColorRGBs->value = zinalize_color_rgb_array_d(color_rgbs);
}


// Builds Colored Vox Meshes
// When: ChunkMeshDirty is zox_dirty_active
void ChunkColorsBuildSystem(iter *it) {
    zox_ts_begin(build_chunk_colored);
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
        zox_sys_i(VoxelNode, voxelNode);
        zox_sys_i(NodeDepth, nodeDepth);
        zox_sys_i(RenderDepth, renderDepth);
        zox_sys_i(ChunkNeighbors, chunkNeighbors);
        zox_sys_i(ColorRGBs, colorRGBs);
        zox_sys_i(ChunkSize, chunkSize);
        zox_sys_i(BlockScale, blockScale);
        zox_sys_o(MeshDirty, mesh_dirty);
        zox_sys_o(MeshIndicies, meshIndicies);
        zox_sys_o(MeshVertices, meshVertices);
        zox_sys_o(MeshColorRGBs, meshColorRGBs);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!colorRGBs->length) {
            zox_sys_e();
            zox_logw("Vox has no colors [%s]", zox_get_name(e));
            continue;
        }

        // removes mesh when 255
        clear_mesh(meshIndicies, meshVertices, meshColorRGBs);
        if (renderDepth->value >= render_depth_spawning) {
            mesh_dirty->value = mesh_state_trigger_slow;
            continue;
        }

        // fetch neighbor depths and nodes
        const VoxelNode* neighbors[6];
        byte nrender_depths[6];
        for (int j = 0; j < 6; j++) {
            entity n = chunkNeighbors->value[j];
            neighbors[j] = n ? zox_gett(chunkNeighbors->value[j], VoxelNode) : NULL;
            nrender_depths[j] = n ? zox_gett_value(n, RenderDepth) : 0;
        }

        const byte node_depth = nodeDepth->value;
        const byte render_depth = renderDepth->value;   // this needs to be set better, per model type
        const float3 b = calculate_vox_bounds(
            chunkSize->value,
            blockScale->value);

        const float3 offset = float3_scale(b, -1);
        byte chunk_length = powers_of_two[node_depth];    // render_depth
        const float chunk_scale = blockScale->value * chunk_length;

        read_lock_VoxelNode(voxelNode);

        build_node_mesh_colors(
            voxelNode,
            colorRGBs,
            meshIndicies,
            meshVertices,
            meshColorRGBs,
            render_depth,
            neighbors,
            nrender_depths,
            offset,
            chunk_scale);

        read_unlock_VoxelNode(voxelNode);

        mesh_dirty->value = mesh_state_trigger_slow;
    }
    zox_ts_end(build_chunk_colored, 5, zox_profile_system_chunk_builder_c);
} zoxd_system(ChunkColorsBuildSystem)



        /*const VoxelNode *chunk_right = chunkNeighbors->value[1] == 0 ? NULL : zox_get(chunkNeighbors->value[1], VoxelNode)
        const VoxelNode *chunk_down = chunkNeighbors->value[2] == 0 ? NULL : zox_get(chunkNeighbors->value[2], VoxelNode)
        const VoxelNode *chunk_up = chunkNeighbors->value[3] == 0 ? NULL : zox_get(chunkNeighbors->value[3], VoxelNode)
        const VoxelNode *chunk_back = chunkNeighbors->value[4] == 0 ? NULL : zox_get(chunkNeighbors->value[4], VoxelNode)
        const VoxelNode *chunk_front = chunkNeighbors->value[5] == 0 ? NULL : zox_get(chunkNeighbors->value[5], VoxelNode)
        const VoxelNode *neighbors[] =  { chunk_left, chunk_right, chunk_down, chunk_up, chunk_back, chunk_front };

        const byte chunk_right_render_depth = chunkNeighbors->value[1] == 0 ? 0 : zox_get_value(chunkNeighbors->value[1], RenderDepth)
        const byte chunk_down_render_depth = chunkNeighbors->value[2] == 0 ? 0 : zox_get_value(chunkNeighbors->value[2], RenderDepth)
        const byte chunk_up_render_depth = chunkNeighbors->value[3] == 0 ? 0 : zox_get_value(chunkNeighbors->value[3], RenderDepth)
        const byte chunk_back_render_depth = chunkNeighbors->value[4] == 0 ? 0 : zox_get_value(chunkNeighbors->value[4], RenderDepth)
        const byte chunk_front_render_depth = chunkNeighbors->value[5] == 0 ? 0 : zox_get_value(chunkNeighbors->value[5], RenderDepth)*/

        /*nrender_depths[0] = get_render_depth_from_lod(chunk_left_render_depth, node_depth);
        nrender_depths[1] = get_render_depth_from_lod(chunk_right_render_depth, node_depth);
        nrender_depths[2] = get_render_depth_from_lod(chunk_down_render_depth, node_depth);
        nrender_depths[3] = get_render_depth_from_lod(chunk_up_render_depth, node_depth);
        nrender_depths[4] = get_render_depth_from_lod(chunk_back_render_depth, node_depth);
        nrender_depths[5] = get_render_depth_from_lod(chunk_front_render_depth, node_depth);*/
