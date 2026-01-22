// Doing: Keeping in one place, making iteration easier

// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

byte zox_disable_node_face_subdivision = 1;

// remember: vertex offset is just node position / voxel position

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

// this function accounts for size of drawing voxels
void build_voxel_mesh_final(terrain_build_data data, octree_dig_data dig, octree_face_data face) {
    // data.render_depth | dig.depth | adepth
    const byte adepth = get_adjacent_depth_VoxelNode(
        data.neighbors,
        data.ndepths,
        dig.position,
        data.render_depth,
        dig.direction
    );
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
    ddepth++;


    byte asolid = anode && anode->value && data.voxel_solidity[anode->value - 1];

    // NOTE: A special case here if neighbor is lesser / higher lod
    if (asolid && adepth > dig.depth) {
        asolid = get_node_sides_all_solid(
            data.voxel_solidity,
            anode,
            rdir,
            dig.depth
        );
    }

    // we need to know how far to check, using anodes depth
    /*byte asolid = get_node_sides_all_solid(
        data.voxel_solidity,
        anode,
        rdir,
        ddepth);
    asolid = anode ? asolid : data.edge_voxel;*/

    if (!asolid) {
        ((VoxelNode*) dig.node)->sides |= (1 << dig.direction);
        zox_build_voxel_face(
            data.mesh_data,
            face.indicies,
            face.vertices,
            face.uvs,
            dig.offset,
            dig.scale
        );
    }
}

void zox_terrain_building_dig(terrain_build_data data, octree_dig_data dig) {
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
    byte render_depth,
    const VoxelNode **neighbors,
    const byte* ndepths,
    const byte *voxel_solidity,
    const int *voxel_uv_indexes,
    float scale
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

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
zox_sys2(Chunk3TexturedBuildSystem) {
    if (zox_chunk3_texture_builder_new) {
        return;
    }
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
    if (voxels_length == 0) {
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
        const entity block = blocks->value[i];
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
        const byte block_textures_length = block_textures->length;
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
        zox_sys_i(BlockScale, block_scale);
        zox_sys_i(VoxelNode, voxel_node);
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
            rdepth->value == terrain_depth) {
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

        const VoxelNode *nnodes[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(world, neighbors, nnodes, ndepths);

        const byte render_depth = rdepth->value;
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

        mdirty->value = mesh_state_trigger_slow;
        // mesh_state_trigger_slow mesh_state_trigger

        // zox_log("Building Terrain Chunk! Verts [%i] Scale [%f] Depth [%i]", verts->length, chunk_scale, render_depth);
    }

} zox_sys_end(Chunk3TexturedBuildSystem);