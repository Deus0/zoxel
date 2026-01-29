// Doing: Keeping in one place, making iteration easier

// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

byte zox_disable_node_face_subdivision = 1;

// remember: vertex position is just node position / voxel position

// static data
typedef struct {
    // chunk data
    const VoxelNode *root;
    const byte rdepth;
    const mesh_uvs_build_data *mesh_data;
    // neighbor data
    // const VoxelNode **neighbors;
    // const byte *ndepths;
    // vox data
    // const float scale; // overall scale
    byte edge_voxel;
    // voxels
    const TilemapUVs *tilemap_uvs;
    const byte *voxel_solidity;
    const int *voxel_uv_indexes;
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
    float3 positionf;
    byte3 local_position;
} octree_dig_data;



// NOTE: A special case here if neighbor is lesser / higher

        // NOTE: For some reason I had to add 1 here, but is fine in other system, the main diff is the dig depth vs render depth
        // What we really need is adjacent node depth vs dig depth, not render depth differences
        // actually this makes sense: we are just checking what neighbor is rendering at verse what we are

// this function accounts for size of drawing voxels
static inline void build_voxel_mesh_final(terrain_build_data data, octree_dig_data dig, octree_face_data face) {

    /*byte sides = dig.node->sides;
    if (!(sides & (1 << dig.direction + 1))) {
        return;
    }*/


    if (!zox_chunk3t_split) {

        zox_build_voxel_face(
            data.mesh_data,
            face.indicies,
            face.vertices,
            face.uvs,
            dig.positionf,
            float3_single(dig.scale)
        );

        return;
    }

    // XZ for now
    float dividor = (float) powers_of_two[(data.rdepth - dig.depth)];
    float qsize = dig.scale / dividor;
    float3 positionf2 = dig.positionf;

    if (dig.direction == direction_down || dig.direction == direction_up) {

        float3 qscale = (float3) { qsize, dig.scale, qsize };

        for (float x = 0; x < dig.scale; x += qsize) {
            positionf2.x = dig.positionf.x + x;

            for (float z = 0; z < dig.scale; z += qsize) {
                positionf2.z = dig.positionf.z + z;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    } else if (dig.direction == direction_left || dig.direction == direction_right) {

        float3 qscale = (float3) { dig.scale, qsize, qsize };

        for (float y = 0; y < dig.scale; y += qsize) {
            positionf2.y = dig.positionf.y + y;

            for (float z = 0; z < dig.scale; z += qsize) {
                positionf2.z = dig.positionf.z + z;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    } else if (dig.direction == direction_back || dig.direction == direction_front) {

        float3 qscale = (float3) { qsize, qsize, dig.scale };

        for (float x = 0; x < dig.scale; x += qsize) {
            positionf2.x = dig.positionf.x + x;

            for (float y = 0; y < dig.scale; y += qsize) {
                positionf2.y = dig.positionf.y + y;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    }
}

static inline void zox_terrain_building_dig(terrain_build_data data, octree_dig_data dig, const SidesOctree* sides) {

    // Dig Deeper
    if (dig.depth < data.rdepth &&
        !is_closed_VoxelNode(dig.node) &&
        !is_closed_SidesOctree(sides)) {

        const VoxelNode* kids = get_children_VoxelNode(dig.node);
        const SidesOctree* sides_kids = get_children_SidesOctree(sides);

        byte child_depth = dig.depth + 1;
        float child_scale = dig.scale * 0.5f;
        int3 position = dig.position;
        int3_multiply_int_p(&position, 2);


        for (byte i = 0; i < 8; i++) {

            /*if (!kids[i].value) {
                continue;
            }*/

            int3 cposition = int3_add(position, octree_positions[i]);
            octree_dig_data child = {
                .parent = dig.node,
                .depth = child_depth,
                .scale = child_scale,
                // unique
                .node = &kids[i],
                .index = i,
                .position = cposition
            };

            zox_terrain_building_dig(data, child, &sides_kids[i]);
        }

        return;
    }

    if (!sides->value) {
        return;
    }

    dig.voxel = dig.node->value;
    dig.positionf = float3_from_int3(dig.position);
    float3_scale_p(&dig.positionf, dig.scale);
    dig.local_position = octree_positions_b[dig.index];
    uint uvindex = (dig.voxel - 1) * 6;

    for (dig.direction = 0; dig.direction < 6; dig.direction++) {

        if (!(sides->value & (1 << dig.direction + 1))) {
            continue;
        }

        uint uv_index = data.voxel_uv_indexes[uvindex + dig.direction];
        octree_face_data face = {
            .indicies = voxel_face_indicies_n + dig.direction * voxel_face_indicies_length,
            .vertices = voxel_face_vertices_n[dig.direction],
            .uvs = &data.tilemap_uvs->value[uv_index],
        };

        build_voxel_mesh_final(data, dig, face);
    }
}

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
zox_sys2(Chunk3TexturedBuildSystem) {
    if (zox_chunk3t_mode == zox_chunk3t_mode_new) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(TilemapLink);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(RenderDepth);
    // zox_sys_in(ChunkNeighbors);
    zox_sys_in(BlockScale);
    zox_sys_in(VoxelNode);
    zox_sys_in(SidesOctree);
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
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(BlockScale, bscale);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(SidesOctree, sides);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_o(MeshDirty, mdirty);

        if (cdirty->value != zox_dirty_active) {
            continue;
        }

        // We have 3 modes, old, new, and hybrid
        if (zox_chunk3t_mode == zox_chunk3t_mode_mix &&
            rdepth->value == terrain_depth) {
            continue;
        }

        // No Mesh Sides were found
        if (!sides->value) {
            clear_mesh_uvs(indicies, verts, colors, uvs);
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

        byte vlength = powers_of_two[rdepth->value];
        float cscale = bscale->value * vlength;

        mesh_uvs_build_data mesh_data = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .uvs = create_float2_array_d(initial_dynamic_array_size),
            .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
        };

        // build out mesh data
        terrain_build_data data = {
            // vox data
            .tilemap_uvs = tilemap_uvs,
            .voxel_solidity = build_data.solidity,
            .voxel_uv_indexes = build_data.uvs,
            .mesh_data = &mesh_data,
            // chunk data
            .root = voctree,
            .rdepth = rdepth->value,
        };

        octree_dig_data dig = {
            .parent = NULL,
            .node = voctree,
            .scale = cscale,
        };

        read_lock_VoxelNode(voctree);

            zox_terrain_building_dig(data, dig, sides);

        read_unlock_VoxelNode(voctree);

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

        // dirty
        mdirty->value = mesh_state_trigger_slow;
        // mesh_state_trigger_slow mesh_state_trigger

        // zox_log("Building Terrain Chunk! Verts [%i] Scale [%f] Depth [%i]", verts->length, cscale, rdepth);
    }

    free_chunk3_textured_builder_data(build_data);
} zox_sys_end(Chunk3TexturedBuildSystem);