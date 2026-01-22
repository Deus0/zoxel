// This one Builds Top Down, rather than by digging
zox_sys2(Chunk3TexturedHighBuildSystem) {
    if (zox_chunk3_texture_builder_old) {
        return;
    }
#ifndef zox_disable_hidden_terrain_edge
    byte edge_voxel = 1;
#else
    byte edge_voxel = 1;
#endif
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

    // Caches Block Data
    // #################
    int voxels_length = 0;
    entity manager = 0;

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, blocker);
        if (!blocker->value) {
            continue;
        }
        manager = blocker->value;
        break;
    }

    if (!manager) {
        return;
    }

    zox_geter(manager, BlockLinks, blocks);
    voxels_length = blocks->length;
    if (!voxels_length) {
        return; // if failed to find terrain parents
    }

    chunk3_textured_builder_data build_data;
    byte solidity[voxels_length];
    build_data.solidity = solidity;
    int uvs[voxels_length * 6]; //  * sizeof(int)];
    build_data.uvs = uvs;
    // calculate tileuv indexes - voxel and face to index  in tilemap_uvs
    int uvs_index = 0;

    for (int i = 0; i < voxels_length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block)) {
            build_data.solidity[i] = 1;
            continue;
        }

        // solidity
        if (!zox_has(block, BlockModel)) {
            build_data.solidity[i] = 1;
        } else {
            build_data.solidity[i] = zox_gett_value(block, BlockModel) == zox_block_solid;
        }

        // Cache the UVs if exist
        if (!zox_has(block, TextureLinks)) {
            continue;
        }

        zox_geter(block, TextureLinks, block_textures);
        byte block_textures_length = block_textures->length;
        int voxel_uv_indexes_index = i * 6;
        if (block_textures_length == 1) {
            // per voxel, 24 uvs
            for (byte k = 0; k < 6; k++) {
                build_data.uvs[voxel_uv_indexes_index + k] = uvs_index;
                // uvs_index += 4;
            }
            uvs_index += 4;
        } else {
            // for 6 sides textured voxes
            for (byte k = 0; k < 6; k++) {
                build_data.uvs[voxel_uv_indexes_index + k] = uvs_index;
                uvs_index += 4;
            }
        }
    }
    // #################

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
        if (zox_chunk3_texture_builder_mix &&
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
                    if (!solidity[node->value - 1]) {
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

                        byte ddepth = adepth - rdepth->value < 0 ? 0 : adepth - rdepth->value;
                        ddepth++;

                        // get anode at the current dig depth
                        const VoxelNode* anode = get_adjacentn_VoxelNode(
                            noctrees,
                            vnode,
                            byte3_to_int3(position),
                            rdepth->value,
                            direction
                        );

                        byte asolid = anode && anode->value && solidity[anode->value - 1];

                        // NOTE: A special case here if neighbor is lesser / higher lod
                        if (asolid && adepth > rdepth->value) {
                            asolid = get_node_sides_all_solid(
                                solidity,
                                anode,
                                rdir,
                                ddepth
                            );
                        }


                        // we need to know how far to check, using anodes depth
                        /*byte asolid = get_node_sides_all_solid(
                            solidity,
                            anode,
                            rdir,
                            ddepth
                        );
                        asolid = anode ? asolid : edge_voxel;*/

                        // If Build Face
                        if (!asolid) {

                            // Cache for Lighting
                            ((VoxelNode*) node)->sides |= (1 << direction);

                            octree_face_data face = {
                                .indicies = voxel_face_indicies_n + direction * voxel_face_indicies_length,
                                .vertices = voxel_face_vertices_n[direction],
                                .uvs = &tilemap_uvs->value[uv_index],
                            };

                            zox_build_voxel_face(
                                &mdata,
                                face.indicies,
                                face.vertices,
                                face.uvs,
                                byte3_to_float3(position),
                                bscale->value
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

} zox_sys_end(Chunk3TexturedHighBuildSystem);