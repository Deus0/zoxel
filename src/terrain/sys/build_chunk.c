// Doing: Keeping in one place, making iteration easier

// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

byte zox_disable_node_face_subdivision = 1;
define_fun_stopwatch(time_chunk3_build, 0);

// remember: vertex offset is just node position / voxel position

// #define zox_octree_chunk_build_limits

// static data
typedef struct {
    // vox data
    // const float scale; // overall scale
    byte edge_voxel;
    // voxels
    const TilemapUVs *tilemap_uvs;
    const byte *voxel_solidity;
    const int *voxel_uv_indexes;
    // neighbor data
    const VoxelNode **neighbors;
    const byte *ndepths;
    // chunk data
    const VoxelNode *root;
    // const byte is_max_depth_chunk;  // is chunk highest lod
    const byte render_depth;
    const byte node_depth;
    const mesh_uvs_build_data *mesh_data;
} terrain_build_data;

// changing data
typedef struct {
    // node data
    const VoxelNode *parent;
    const VoxelNode *node;
    byte depth;
    int3 position;
    float scale;
    byte index;
    byte voxel;
    // face data
    byte direction;
    float3 offset;
    byte3 local_position;
} octree_dig_data;

typedef struct {
    const int* indicies;
    const float3 *vertices;
    const float2 *uvs;
} octree_face_data;

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void zox_build_voxel_face(
    const mesh_uvs_build_data *mesh_data,
    const int* voxel_face_indicies,
    const float3* voxel_face_vertices,
    const float2* voxel_face_uvs,
    const float3 offset,
    const float scale
) {
    // indicies
    expand_capacity_int_array_d(mesh_data->indicies, voxel_face_indicies_length);
    for (byte i = 0; i < 6; i++) {
        mesh_data->indicies->data[mesh_data->indicies->size + i] = mesh_data->vertices->size + voxel_face_indicies[i];
    }
    mesh_data->indicies->size += voxel_face_indicies_length;
    // verts
    expand_capacity_float3_array_d(mesh_data->vertices, voxel_face_vertices_length);
    for (byte i = 0; i < voxel_face_vertices_length; i++) {
        float3 vertex_position = voxel_face_vertices[i];
        float3_add_float3_p(&vertex_position, offset);
        float3_scale_p(&vertex_position, scale);
        mesh_data->vertices->data[mesh_data->vertices->size + i] = vertex_position;
    }
    mesh_data->vertices->size += voxel_face_vertices_length;
    // uvs
    expand_capacity_float2_array_d(mesh_data->uvs, voxel_face_vertices_length);
    for (byte i = 0; i < 4; i++) {
        const float2 vert_uv = voxel_face_uvs[i];
        mesh_data->uvs->data[mesh_data->uvs->size + i] = vert_uv;
    }
    mesh_data->uvs->size += voxel_face_vertices_length;
    // colors
    for (byte a = 0; a < voxel_face_vertices_length; a++) {
        add_to_color_rgb_array_d(mesh_data->color_rgbs, color_rgb_white);
    }
}

// this function accounts for size of drawing voxels
void build_voxel_mesh_final(
    const terrain_build_data data,
    octree_dig_data dig,
    octree_face_data face
) {
    // data.render_depth | dig.depth | adepth
    const byte adepth = get_adjacent_depth_VoxelNode(
        data.neighbors,
        data.ndepths,
        dig.position,
        data.render_depth,
        dig.direction);
    // get anode at the current dig depth
    const VoxelNode* anode = get_adjacentn_VoxelNode(
        data.neighbors,
        data.root,
        dig.position,
        dig.depth,
        dig.direction
    );
    byte rdir = reverse_direction(dig.direction);
    // but this assume the adjacent node is rendering at max level!
    //  it checks for all sub nodes..!
    byte ddepth = adepth - dig.depth < 0 ? 0 : adepth - dig.depth;

    // we need to know how far to check, using anodes depth
    byte adjacent_solid = get_node_sides_all_solid(
        data.voxel_solidity,
        anode,
        rdir,
        ddepth + 1);
    adjacent_solid = anode ? adjacent_solid : data.edge_voxel;

    if (!adjacent_solid) {
        ((VoxelNode*) dig.node)->sides |= (1 << dig.direction);
        zox_build_voxel_face(
            data.mesh_data,
            face.indicies,
            face.vertices,
            face.uvs,
            dig.offset,
            dig.scale);
    }
}

void zox_terrain_building_dig(
    const terrain_build_data data,
    octree_dig_data dig
) {
    ((VoxelNode*) dig.node)->sides = 0; // reset before updating

    if (dig.depth >= data.render_depth || is_closed_VoxelNode(dig.node)) {

        // we dig until depth is at render level or the node is closed
        if (dig.node->value && data.voxel_solidity[dig.node->value - 1]) {

            dig.voxel = dig.node->value;
            dig.offset = float3_from_int3(dig.position);
            dig.local_position = octree_positions_b[dig.index];
            const int voxel_uvs_index = (dig.voxel - 1) * 6;

            for (byte i = 0; i < 6; i++) {
                dig.direction = i;
                int uv_index = data.voxel_uv_indexes[voxel_uvs_index + i];
                octree_face_data face = {
                    .indicies = voxel_face_indicies_n + i * voxel_face_indicies_length,
                    .vertices = voxel_face_vertices_n[i],
                    .uvs = &data.tilemap_uvs->value[uv_index],
                };

                build_voxel_mesh_final(data, dig, face);
            }

        }

    } else {
        // keep digging
        byte child_depth = dig.depth + 1;
        float child_scale = dig.scale * 0.5f;
        int3 position = dig.position;
        int3_multiply_int_p(&position, 2);
        // only dig for solid child nodes
        VoxelNode* kids = get_children_VoxelNode(dig.node);
        for (byte i = 0; i < 8; i++) {
            if (!kids[i].value) {
                continue;
            }
            int3 child_position = int3_add(position, octree_positions[i]);
            octree_dig_data child = {
                .parent = dig.node,
                .depth = child_depth,
                .scale = child_scale,
                // unique
                .node = &kids[i],
                .index = i,
                .position = child_position
            };
            zox_terrain_building_dig(data, child);
        }
    }
}

void build_chunk_terrain_mesh(
    const VoxelNode *node,
    const TilemapUVs *tilemap_uvs,
    MeshIndicies *indicies,
    MeshVertices *verts,
    MeshUVs *uvs,
    MeshColorRGBs *colors,
    // const byte is_max_depth_chunk,
    const byte render_depth,
    const VoxelNode **neighbors,
    const byte* ndepths,
    const byte *voxel_solidity,
    const int *voxel_uv_indexes,
    const float scale
) {
    const mesh_uvs_build_data mesh_data = {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .uvs = create_float2_array_d(initial_dynamic_array_size),
        .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
    };
    // build out mesh data
    const terrain_build_data data = {
        // vox data
        .tilemap_uvs = tilemap_uvs,
        .voxel_solidity = voxel_solidity,
        .voxel_uv_indexes = voxel_uv_indexes,
        .mesh_data = &mesh_data,
        // neighbor data
        .neighbors = neighbors,
        .ndepths = ndepths,
        // chunk data
        .root = node,
        .render_depth = render_depth,
#ifndef zox_disable_hidden_terrain_edge
        .edge_voxel = 1,
#endif
    };
    octree_dig_data dig = {
        .parent = NULL,
        .node = node,
        .scale = scale,
    };
    zox_terrain_building_dig(data, dig);
    // sizes
    indicies->length = mesh_data.indicies->size;
    verts->length = mesh_data.vertices->size;
    uvs->length = mesh_data.uvs->size;
    colors->length = mesh_data.color_rgbs->size;
    // data
    indicies->value = zinalize_int_array_d(mesh_data.indicies);
    verts->value = zinalize_float3_array_d(mesh_data.vertices);
    colors->value = zinalize_color_rgb_array_d(mesh_data.color_rgbs);
    uvs->value = zinalize_float2_array_d(mesh_data.uvs);
}

void fetch_neightbor_chunk_data(
    ecs* world,
    const ChunkNeighbors* chunk_neighbors,
    const VoxelNode** neighbors,
    byte* ndepths
) {
    for (int i = 0; i < 6; i++) {
        const entity e = chunk_neighbors->value[i];
        if (!zox_valid(e) || !zox_has(e, RenderDepth) || !zox_has(e, VoxelNode)) {
            ndepths[i] = 0;
            neighbors[i] = 0;
            continue;
        }
        neighbors[i] = zox_get(e, VoxelNode);
        zox_geter_value(e, RenderDepth, byte, render_depth);
        ndepths[i] = render_depth;
    }
}

// TODO: Move terrain cache into functions
void Chunk3BuildSystem(iter *it) {
    zox_ts_begin(chunk3_builder);
    uint updated_count = 0;

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxLink);
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

    byte any_dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, chunk_mesh_dirty)
        if (chunk_mesh_dirty->value == zox_dirty_active) {
            any_dirty = 1;
            break;
        }
    }
    if (!any_dirty) {
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return;
    }

    startwatch(time_chunk3_build);

    // Cache terrain data
    int voxels_length = 0;
    entity terrain = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxLink, voxLink);
        if (!voxLink->value) {
            continue;
        }
        terrain = voxLink->value;
        break;
    }
    if (!zox_valid(terrain) || !zox_has(terrain, RealmLink) || !zox_has(terrain, TilemapLink)) {
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return;
    } // if failed to find terrain parents

    const entity realm = zox_get_value(terrain, RealmLink);
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return;
    }

    zox_geter(realm, BlockLinks, blocks);
    voxels_length = blocks->length;
    if (voxels_length == 0) {
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return; // if failed to find terrain parents
    }
    const entity tilemap = zox_get_value(terrain, TilemapLink);
    if (!zox_valid(tilemap) || !zox_has(tilemap, TilemapUVs)) {
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return;
    }
    zox_geter(tilemap, TilemapUVs, tilemap_uvs);
    if (tilemap_uvs->value == NULL || tilemap_uvs->length == 0) {
        // zox_log(" ! tilemap troubles in chunk building: %lu %i\n", tilemap, tilemap_uvs->length)
        zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);
        return; // if tilemap generating still
    }
    ChunkTexturedBuildData build_data;
    byte solidity[voxels_length];
    build_data.solidity = solidity;
    int uvs[voxels_length * 6]; //  * sizeof(int)];
    build_data.uvs = uvs;
    // calculate tileuv indexes - voxel and face to index  in tilemap_uvs
    int uvs_index = 0;
    for (int j = 0; j < voxels_length; j++) {
        const entity block = blocks->value[j];
        if (!zox_valid(block)) {
            build_data.solidity[j] = 1;
            continue;
        }
        // solidity
        if (!zox_has(block, BlockModel)) {
            build_data.solidity[j] = 1;
        } else {
            build_data.solidity[j] = zox_gett_value(block, BlockModel) == zox_block_solid;
        }
        // uvs
        if (!zox_has(block, TextureLinks)) {
            continue;    // no textures...!
        }
        const byte block_textures_length = zox_gett(block, TextureLinks)->length;
        int voxel_uv_indexes_index = j * 6;
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


    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, chunk_mesh_dirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(BlockScale, block_scale);
        zox_sys_i(VoxelNode, voxel_node);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_o(MeshDirty, mesh_dirty);

        if (chunk_mesh_dirty->value != zox_dirty_active) {
            continue;
        }

        if (rdepth->value == render_depth_spawning)  {
            zox_log_error("render_depth_uninitialized");
            continue;
        }

        clear_mesh_uvs(
            indicies,
            verts,
            colors,
            uvs
        );

        if (rdepth->value == render_depth_invisible) {
            mesh_dirty->value = mesh_state_trigger_slow;
            continue;
        }

        const VoxelNode *nnodes[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(
            world,
            neighbors,
            nnodes,
            ndepths);

        const byte render_depth =  rdepth->value;
        const float chunk_scale = block_scale->value * powers_of_two[render_depth];

        read_lock_VoxelNode(voxel_node);
        build_chunk_terrain_mesh(
            voxel_node,
            tilemap_uvs,
            indicies,
            verts,
            uvs,
            colors,
            render_depth,
            nnodes,
            ndepths,
            build_data.solidity,
            build_data.uvs,
            chunk_scale
        );
        read_unlock_VoxelNode(voxel_node);

        mesh_dirty->value = mesh_state_trigger; // mesh_state_trigger_slow;

        // zox_sys_e();
        // zox_log("built chunk mesh [%s]", zox_get_name(e));

        tapwatch(time_chunk3_build, "built mesh");
        updated_count++;

        /*zox_log("Building Terrain Chunk! Verts [%i] Scale [%f] Depth [%i]",
                verts->length,
                chunk_scale,
                render_depth);*/
    }

    /*if (updated_count > 0) {
        zox_log_v(" - [%i] updated [%i]", ecs_run_count, updated_count)
    }*/
    endwatch(time_chunk3_build, "ending");
    zox_ts_end(chunk3_builder, 3, zox_profile_system_chunk3_builder);

} zoxd_system2(Chunk3BuildSystem);



    // We split up Lower Depth Nodes, and draw all Visible Quads
    /*byte is_maxxed = data.render_depth == dig.depth;
    if (!zox_disable_node_face_subdivision && !is_maxxed) {
        // so far just increasing face draw resolution for up faces
        int depth_difference = data.render_depth - dig.depth;
        if (depth_difference != 0) {
            is_regular_build = 0;
            // this checks per voxel position if voxel is solid next to it
            int3 scaled_octree_position = dig.position;
            const int amplify_position = pow(2, depth_difference);
            if (amplify_position != 1) {
                int3_multiply_int_p(&scaled_octree_position, amplify_position);
            }
            int3 position = int3_zero;
            // place at top of current scale building
            if (dig.direction == direction_up) {
                position.y = (dig.scale - 1);
            } else if (dig.direction == direction_right) {
                position.x = (dig.scale - 1);
            } else if (dig.direction == direction_front) {
                position.z = (dig.scale - 1);
            }
            // three edge cases
            if (dig.direction == direction_up || dig.direction == direction_down) { // y
                for (position.x = 0; position.x < dig.scale; position.x++) {
                    for (position.z = 0; position.z < dig.scale; position.z++) {
                        int3 global_octree_position = scaled_octree_position;
                        int3_add_int3(&global_octree_position, position);
                        if (is_adjacent_solid(
                            dig.direction,
                            data.root,
                            data.neighbors,
                            global_octree_position,
                            data.render_depth,
                            data.edge_voxel,
                            data.voxel_solidity)
                        ) {
                            continue;
                        }
                        zox_build_voxel_face(
                            data.mesh_data,
                            dig.voxel,
                            face.indicies,
                            face.vertices,
                            face.uvs,
                            dig.direction,
                            float3_from_int3(global_octree_position),
                            data.scale);
                    }
                }
            } else if (dig.direction == direction_left || dig.direction == direction_right) { // x
                for (position.y = 0; position.y < dig.scale; position.y++) {
                    for (position.z = 0; position.z < dig.scale; position.z++) {
                        int3 global_octree_position = scaled_octree_position;
                        int3_add_int3(&global_octree_position, position);
                        if (is_adjacent_solid(
                            dig.direction,
                            data.root,
                            data.neighbors,
                            global_octree_position,
                            data.render_depth,
                            data.edge_voxel,
                            data.voxel_solidity)
                        ) {
                            continue;
                        }
                        zox_build_voxel_face(
                            data.mesh_data,
                            dig.voxel,
                            face.indicies,
                            face.vertices,
                            face.uvs,
                            dig.direction,
                            float3_from_int3(global_octree_position),
                            data.scale);
                    }
                }
            } else { // z
                for (position.x = 0; position.x < dig.scale; position.x++) {
                    for (position.y = 0; position.y < dig.scale; position.y++) {
                        int3 global_octree_position = scaled_octree_position;
                        int3_add_int3(&global_octree_position, position);
                        if (is_adjacent_solid(
                            dig.direction,
                            data.root,
                            data.neighbors,
                            global_octree_position,
                            data.render_depth,
                            data.edge_voxel,
                            data.voxel_solidity)
                        ) {
                            continue;
                        }
                        zox_build_voxel_face(
                            data.mesh_data,
                            dig.voxel,
                            face.indicies,
                            face.vertices,
                            face.uvs,
                            dig.direction,
                            float3_from_int3(global_octree_position),
                            data.scale);
                    }
                }
            }
        }
    }*/

/*#ifndef zox_disable_fake_voxel_lighting
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
#endif*/