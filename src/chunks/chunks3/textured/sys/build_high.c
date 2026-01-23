// NOTE: It's much easier to debug tthis as its more self contained

// This one Builds Top Down, rather than by digging
zox_sys2(Chunk3TexturedHighBuildSystem) {
    if (zox_chunk3t_mode == zox_chunk3t_mode_old) {
        return;
    }
    byte edge_voxel = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(TilemapLink);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(VoxelNode);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(BlockScale);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_out(MeshColorRGBs);
    zox_sys_out(MeshDirty);

    // Does a sweep of states first
    byte any_dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, cdirty)
        if (cdirty->value == zox_dirty_active) {
            any_dirty = 1;
            break;
        }
    }

    if (!any_dirty) {
        return;
    }

    chunk3_textured_builder_data build_data;
    if (!cache_blocks_data(it, &build_data)) {
        return;
    }

    // Our Loop
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TilemapLink, tilemap);
        zox_sys_i(ChunkMeshDirty, cdirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(BlockScale, bscale);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_o(MeshDirty, mdirty);

        if (cdirty->value != zox_dirty_active) {
            continue;
        }

        if (rdepth->value == render_depth_spawning)  {
            zox_log_error("render_depth_uninitialized");
            continue;
        }

        // We have 3 modes, old, new, and hybrid
        if (zox_chunk3t_mode == zox_chunk3t_mode_mix &&
            rdepth->value != terrain_depth) {
            continue;
        }

        clear_mesh_uvs(indicies, verts, colors, uvs        );

        if (rdepth->value == render_depth_invisible) {
            mdirty->value = mesh_state_trigger_slow;
            continue;
        }

        // const entity tilemap = zox_get_value(manager, TilemapLink);
        if (!zox_valid(tilemap->value) || !zox_has(tilemap->value, TilemapUVs)) {
            zox_sys_e();
            zox_log_error("Tilemap not found on Chunk Terrain [%s]", zox_get_name(e));
            continue;
        }

        zox_geter(tilemap->value, TilemapUVs, tilemap_uvs);
        if (!tilemap_uvs->value || !tilemap_uvs->length) {
            zox_sys_e();
            zox_log_error("Tilemap busy on Chunk Terrain [%s]", zox_get_name(e));
            continue;
        }

        byte3 position;
        byte vlength = powers_of_two[rdepth->value];
        const VoxelNode *noctrees[6];
        byte ndepths[6];

        fetch_neightbor_chunk_data(world, neighbors, noctrees, ndepths);

        mesh_uvs_build_data mdata = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .uvs = create_float2_array_d(initial_dynamic_array_size),
            .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
        };


        // TODO: I should do this just before reading, as we might read from neighbor nodes too
        // read_lock_VoxelNode(vnode);

        for (position.x = 0; position.x < vlength; position.x++) {
            for (position.y = 0; position.y < vlength; position.y++) {
                for (position.z = 0; position.z < vlength; position.z++) {

                    // get_##T(const T* node, byte target_depth, byte3 pos, byte depth)
                    const VoxelNode* node = get_VoxelNode(
                        vnode,
                        rdepth->value,
                        position,
                        0
                    );

                    // If Air, Pass
                    if (!node->value) {
                        continue;
                    }

                    // Non Solid Blocks Pass
                    if (!build_data.solidity[node->value - 1]) {
                        continue;
                    }

                    uint voxel_uvs_index = (node->value - 1) * 6;

                    for (byte direction = 0; direction < 6; direction++) {

                        // if (direction != direction_up) continue;

                        uint uv_index = build_data.uvs[voxel_uvs_index + direction];

                        // Now just check here per sides

                        byte rdir = reverse_direction(direction);

                        // data.render_depth | dig.depth | adepth
                        byte adepth = get_adjacent_depth_VoxelNode(
                            noctrees,
                            ndepths,
                            byte3_to_int3(position),
                            rdepth->value,
                            direction
                        );

                        byte ddepth = adepth - rdepth->value < 0 ? 1 : adepth - rdepth->value + 1;

                        // get anode at the current dig depth
                        const VoxelNode* anode = get_adjacentn_VoxelNode(
                            noctrees,
                            vnode,
                            byte3_to_int3(position),
                            rdepth->value,
                            direction
                        );

                        byte asolid = anode && anode->value && build_data.solidity[anode->value - 1];

                        // NOTE: A special case here if neighbor is lesser / higher lod
                        if (asolid && adepth > rdepth->value) {
                            asolid = get_node_sides_all_solid(
                                build_data.solidity,
                                anode,
                                rdir,
                                ddepth
                            );
                        }

                        // If Build Face
                        if (!asolid) {

                            // Cache for Lighting
                            // ((VoxelNode*) node)->sides |= (1 << direction);

                            octree_face_data face = {
                                .indicies = voxel_face_indicies_n + direction * voxel_face_indicies_length,
                                .vertices = voxel_face_vertices_n[direction],
                                .uvs = &tilemap_uvs->value[uv_index],
                            };

                            float3 positionf = byte3_to_float3(position);
                            float3_scale_p(&positionf, bscale->value);

                            zox_build_voxel_face(
                                &mdata,
                                face.indicies,
                                face.vertices,
                                face.uvs,
                                positionf,
                                float3_single(bscale->value)
                            );
                        }
                    }
                }
            }
        }
        // read_unlock_VoxelNode(vnode);

        // sizes
        indicies->length = mdata.indicies->size;
        verts->length = mdata.vertices->size;
        uvs->length = mdata.uvs->size;
        colors->length = mdata.color_rgbs->size;
        // data
        indicies->value = zinalize_int_array_d(mdata.indicies);
        verts->value = zinalize_float3_array_d(mdata.vertices);
        colors->value = zinalize_color_rgb_array_d(mdata.color_rgbs);
        uvs->value = zinalize_float2_array_d(mdata.uvs);

        mdirty->value = mesh_state_trigger_slow;
    }

    free_chunk3_textured_builder_data(build_data);
} zox_sys_end(Chunk3TexturedHighBuildSystem);