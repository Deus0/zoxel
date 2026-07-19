// Doing: Keeping in one place, making iteration easier
// TODO: Remove structs!!!
// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

byte zox_disable_node_face_subdivision = 1;
// remember: vertex position is just node position / voxel position

typedef struct {
    int_array_d *indicies;
    float3_array_d* vertices;
    float2_array_d* uvs;
    color_rgb_array_d* color_rgbs;
} mesh_uvs_build_data;

typedef struct {
    const int* indicies;
    const float3 *vertices;
    const float2 *uvs;
} octree_face_data;

// static data
typedef struct {
    // chunk data
    const VoxelNode* root;
    const byte rdepth;
    const mesh_uvs_build_data* mesh_data;
    byte edge_voxel;
    // voxels
    const TilemapUVs* tilemap_uvs;
    const byte* voxel_solidity;
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

/*byte cache_blocks_data(iter* it, byte* solidity) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    entity manager = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, blocker);
        if (!zox_valid(blocker->value)) {
            continue;
        }
        manager = blocker->value;
        break;
    }
    if (!manager) {
        return 0;
    }
    zox_geter(manager, BlockLinks, blocks);
    if (!blocks->length) {
        return 0; // if failed to find terrain parents
    }
    solidity = malloc(blocks->length * sizeof(byte));
    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block)) {
            solidity[i] = 1;
            continue;
        }
        // solidity
        if (!zox_has(block, BlockModel)) {
            solidity[i] = 1;
        } else {
            solidity[i] = zox_gett_value(block, BlockModel) == zox_block_solid;
        }
    }
    return 1;
}*/

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void zox_build_voxel_face(const mesh_uvs_build_data* mesh, const int* face_indicies, const float3* face_verts, const float2* face_uvs, float3 position, float3 scale3) {
#ifdef zox_safety_checks
    if (!face_uvs) { // TODO: If we dont generate realm blocks  it goes out of index, we should account for this
        zox_loge("face_uvs is null in zox_build_voxel_face");
        return;
    }
#endif
    for (byte i = 0; i < 6; i++) {
        int index = mesh->vertices->size + face_indicies[i];
        int_array_d_add(mesh->indicies, index);
    }
    for (byte i = 0; i < voxel_face_vertices_length; i++) {
        // int j = mesh->vertices->size + i;
        float3 vert = face_verts[i];
        float3_scale3p(&vert, scale3);
        float3_add_float3_p(&vert, position);
        float3_array_d_add(mesh->vertices, vert);
        float2_array_d_add(mesh->uvs, face_uvs[i]);
        color_rgb_array_d_add(mesh->color_rgbs, color_rgb_white);
    }
}


// NOTE: A special case here if neighbor is lesser / higher
// NOTE: For some reason I had to add 1 here, but is fine in other system, the main diff is the dig depth vs render depth
// What we really need is adjacent node depth vs dig depth, not render depth differences
// actually this makes sense: we are just checking what neighbor is rendering at verse what we are
static inline void zox_terrain_building_dig(terrain_build_data data, octree_dig_data dig, const SidesOctree* sides) {
    // Dig Deeper
    if (dig.depth < data.rdepth && sides->ptr) { // !is_closed_SidesOctree(sides)) {
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        // get_children_SidesOctree(sides);
        byte has_vkids = !is_closed_VoxelNode(dig.node);
        const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(dig.node) : NULL;
        byte child_depth = dig.depth + 1;
        float child_scale = dig.scale * 0.5f;
        int3 position = dig.position;
        int3_multiply_int_p(&position, 2);
        for (byte i = 0; i < 8; i++) {
            const VoxelNode* cvoctree = has_vkids ? &vkids[i] : dig.node;
            int3 cposition = int3_add(position, octree_positions[i]);
            octree_dig_data child = {
                .parent = dig.node,
                .depth = child_depth,
                .scale = child_scale,
                .node = cvoctree,
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
    if (!dig.node->value) {
        // zox_log_error("Sides error, air cannot render.");
        return;
    }
    dig.voxel = dig.node->value;
    dig.positionf = float3_from_int3(dig.position);
    float3_scale_p(&dig.positionf, dig.scale);
    dig.local_position = octree_positions_b[dig.index];
    // NOTE: UVIndex is calculated by block index * 6 for faces
    byte block_index = dig.voxel - 1;
    uint block_uv_index = block_index * 6 * 4;
    byte dbg_log = 0;
    if (dbg_log && block_index == 0) {
        zox_log("-b- Block [%i], UV Index [%i]", block_index, block_uv_index);
    }
    for (dig.direction = 0; dig.direction < 6; dig.direction++) {
        if (!(sides->value & (1 << (dig.direction + 1)))) {
            continue;
        }
        // uint face_uv_index = data.voxel_uv_indexes[block_face_index + dig.direction];
        uint face_uv_index = block_uv_index + dig.direction * 4;
        octree_face_data face = {
            .indicies = voxel_face_indicies_n + dig.direction * voxel_face_indicies_length,
            .vertices = voxel_face_vertices_n[dig.direction],
            .uvs = &data.tilemap_uvs->value[face_uv_index],
        };
        zox_build_voxel_face(data.mesh_data, face.indicies, face.vertices, face.uvs, dig.positionf, float3_single(dig.scale));
    }
}

/*void clear_mesh_uvs(MeshIndicies *meshIndicies, MeshVertices *meshVertices, MeshColorRGBs *meshColorRGBs, MeshUVs *meshUVs) {
    // zox_log("Clearing Chunk Mesh");
    clear_mesh(meshIndicies, meshVertices, meshColorRGBs);
    dispose_MeshUVs(meshUVs);
}*/

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
// NOTE: Rebuilds Chunk when BuildChunkMesh is dirty

// TODO: DIsable when not selected LOD Mesh
zox_sys2(ChunkTexturedBuildSystem) {
    byte dbg_log = 0;
    byte* solidity = NULL;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(BuildChunkMesh);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_out(MeshColorRGBs);
    zox_sys_out(TexturedMeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(BuildChunkMesh, build);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_o(TexturedMeshDirty, upload);
        if (!build->value || upload->value) {
            continue;
        }
        // Get chunk data
        entity chunk = zox_get_parent(world, e);
        if (zox_combine_chunk_mode) {
            chunk = e;
        }
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Invalid");
            continue;
        }
#endif
        // if generating we wait
        if (zox_getv(chunk, GenerateChunk) || zox_getv(chunk, BuildChunkSides)) {
            if (dbg_log) {
                zox_log("Waiting on Chunk to Build [%s]", zox_getn(chunk));
            }
            continue;
        }
        entity tilemap = zox_getv(chunk, TilemapLink);
#ifdef zox_safety_checks
        if (!zox_valid(tilemap) || !zox_has(tilemap, TilemapUVs)) {
            zox_sys_e();
            zox_loge("Tilemap not found on Chunk Terrain [%s]", zox_get_name(e));
            continue;
        }
#endif
        zox_geter(tilemap, TilemapUVs, tilemap_uvs);
        if (!tilemap_uvs->value || !tilemap_uvs->length) {
            zox_loge("Tilemap TilemapUVs on Chunk Terrain [%s] has not generated", zox_get_name(e));
            continue;
        }
        if (!solidity) {
            entity manager = zox_getv(chunk, BlockManagerLink);
            if (!manager) {
                zox_loge("Failed to find BlockManagerLink on Chunk from ChunkMeshes");
                continue;
            }
            zox_geter(manager, BlockLinks, blocks);
            if (!blocks->length) {
                continue; // if failed to find terrain parents
            }
            solidity = malloc(blocks->length);
            memset(solidity, 1, blocks->length);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                if (!zox_valid(block)) {
                    continue;
                }
                // solidity
                if (zox_has(block, BlockModel)) {
                    solidity[j] = zox_getv(block, BlockModel) == zox_block_solid;
                }
            }
        }
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        const SidesOctree* sides = zox_get(chunk, SidesOctree);
        float block_scale = zox_getv(chunk, BlockScale);
        short vlength = octree_size(depth->value);
        float cscale = block_scale * vlength;
        mesh_uvs_build_data mesh_data = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .uvs = create_float2_array_d(initial_dynamic_array_size),
            .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
        };
        // build out mesh data
        terrain_build_data data = {
            .tilemap_uvs = tilemap_uvs,
            .voxel_solidity = solidity,
            .mesh_data = &mesh_data,
            .root = voxels,
            .rdepth = depth->value,
        };
        octree_dig_data dig = {
            .parent = NULL,
            .node = voxels,
            .scale = cscale,
        };
        read_lock_VoxelNode(voxels);
        zox_terrain_building_dig(data, dig, sides);
        read_unlock_VoxelNode(voxels);
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
        build->value = 0;
        upload->value = 1;
        if (dbg_log) {
            zox_log("Built [%s]! Verts [%i] Scale [%f] Depth [%i]", zox_getn(e), verts->length, cscale, depth->value);
        }
        zox_sys_increment();
    }
    if (solidity) {
        free(solidity);
    }
} zox_sys_end(ChunkTexturedBuildSystem);
