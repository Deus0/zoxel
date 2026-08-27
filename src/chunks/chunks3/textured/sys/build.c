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
    const mesh_uvs_build_data* mesh_data;
    const TilemapUVs* tilemap_uvs;
    const byte* voxel_solidity;
} terrain_build_data;

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
static inline void zox_build_voxel_face(
    const mesh_uvs_build_data* mesh,
    const int* face_indicies,
    const float3* face_verts,
    const float2* face_uvs,
    float3 position,
    float3 scale)
{
#ifdef zox_safety_checks
    if (!face_uvs) { // TODO: If we dont generate realm blocks  it goes out of index, we should account for this
        zox_loge("face_uvs is null in zox_build_voxel_face");
        return;
    }
#endif
    // NOTE: Add our triangle indices
    for (byte i = 0; i < 6; i++) {
        int index = mesh->vertices->size + face_indicies[i];
        int_array_d_add(mesh->indicies, index);
    }
    // NOTE: Add our face vertex data
    for (byte i = 0; i < 4; i++) {
        float3 vert = face_verts[i];
        float3_scale3p(&vert, scale);
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
static inline void zox_terrain_building_dig(
    terrain_build_data data,
    const SidesOctree* sides,
    const VoxelNode* voxels,
    byte3 position,
    float scale,
    byte target_depth,
    byte depth,
    byte dbg_log)
{
    // Dig Deeper
    if (depth < target_depth && sides->ptr) {
        depth++;
        scale *= 0.5f;
        position = byte3_mul1(position, 2);
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        byte has_vkids = !is_closed_VoxelNode(voxels);
        const VoxelNode* vkids = has_vkids ? (const VoxelNode*) voxels->ptr : NULL;
        for (byte i = 0; i < 8; i++) {
            const SidesOctree* child_sides = &sides_kids[i];
            const VoxelNode* child_voxels = has_vkids ? &vkids[i] : voxels;
            byte3 child_position = byte3_add(position, octree_positions[i]);
            zox_terrain_building_dig(
                data,
                child_sides,
                child_voxels,
                child_position,
                scale,
                target_depth,
                depth,
                dbg_log);
        }
        return;
    }
    // NOTE: Skips if no faces to draw
    if (!sides->value) {
        return;
    }
    // NOTE: Maybe this is because it didnt build sides before mesh?
    if (!voxels->value) {
        // zox_loge("Terrain Chunk Mesh Builder: Sides is true with Air");
        return;
    }
    /*if (depth != target_depth) {
        zox_loge("Sides didn't reach target depth [%i < %i]", depth, target_depth);
    }*/
    if (dbg_log >= 2) {
        zox_log("Adding Chunk Textured Faces at [%ix%ix%i]", position.x, position.y, position.z);
    }
    float3 positionf = byte3_to_float3(position);
    float3_scale_p(&positionf, scale);
    // NOTE: UVIndex is calculated by block index * 6 for faces
    byte block_index = voxels->value - 1;
    uint block_uv_index = block_index * 6 * 4;
    for (byte direction = 0; direction < 6; direction++) {
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
        uint face_uv_index = block_uv_index + direction * 4;
        octree_face_data face = {
            .indicies = voxel_face_indicies_n + direction * voxel_face_indicies_length,
            .vertices = voxel_face_vertices_n[direction],
            .uvs = &data.tilemap_uvs->value[face_uv_index],
        };
        zox_build_voxel_face(data.mesh_data, face.indicies, face.vertices, face.uvs, positionf, float3_single(scale));
    }
}

// NOTE: Assumes node depth is lower than terrain (max depth)
static inline float get_chunk_scale(
    byte chunk_depth,
    byte terrain_depth,
    float terrain_scale)
{
    if (terrain_depth < chunk_depth) {
        return terrain_scale;
    } else {
        // we multiply by the depth difference power
        // - if 2 = 2*2 = 4, 0.5 becomes 2 in size
        byte ddepth = terrain_depth - chunk_depth;
        return terrain_scale * octree_size(ddepth);
    }
}

extern byte disable_lights;

// TODO: Cache Multiple Voxel Managers, not just single
// NOTE: Rebuilds Chunk when BuildChunkMesh is dirty
zox_sys2(ChunkTexturedBuildSystem) {
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    // zox_log("Chunk Texture Builds [%i]", it->count);
    entity terrain_cache = 0;
    byte solidity[255];
    entity tilemap_cache = 0;
    const TilemapUVs* tilemap_uvs = NULL;
    // NOTE: we  had to do this as there was a array limit on some platforms?
    // byte* solidity = NULL;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_out(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        // Get chunk data
        entity chunk = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Parent Invalid for [%s]", zox_sys_e_name);
            continue;
        }
#endif
        // Chunk Validation
        byte chunk_depth = zox_getv(chunk, NodeDepth);
        if (depth->value > chunk_depth) {
            zox_loge("Chunk Mesh Depth > Chunks [%s] [%i] > [%i]",
                zox_getn(chunk),
                depth->value,
                chunk_depth);
            continue;
        }
        entity manager = zox_getv(chunk, BlockManagerLink);
        entity terrain = zox_get_parent(world, chunk);
#ifdef zox_safety_checks
        if (!zox_valid(terrain) || !zox_valid(manager)) {
            zox_loge("Terrain Invalid for [%s]", zox_getn(chunk));
            continue;
        }
#endif
        if (manager != terrain_cache) {
#ifdef zox_safety_checks
            if (!manager) {
                zox_loge("Failed to find BlockManagerLink on Chunk from ChunkMeshes");
                continue;
            }
#endif
            zox_geter(manager, BlockLinks, blocks);
#ifdef zox_safety_checks
            if (!blocks->length) {
                continue; // if failed to find terrain parents
            }
#endif
            terrain_cache = manager;
            memset(solidity, 1, blocks->length);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                if (!zox_valid(block) ||
                    !zox_has(block, BlockModel))
                {
                    continue;
                }
                solidity[j] = zox_getv(block, BlockModel) == zox_block_solid;
            }
        }
        entity tilemap = zox_getv(chunk, TilemapLink);
        if (!zox_valid(tilemap)) {
            continue;
        }
        if (tilemap_cache != tilemap) {
#ifdef zox_safety_checks
            if (!zox_valid(tilemap) ||
                !zox_has(tilemap, TilemapUVs) ||
                !zox_has(tilemap, GenerateTexture))
            {
                zox_sys_e();
                zox_loge("Tilemap not found on Chunk Terrain [%s]", zox_sys_e_name);
                continue;
            }
#endif
            if (zox_getv(tilemap, GenerateTexture)) {
                if  (dbg_log) {
                    zox_log("Tilemap Still Generating [%s]: %i",
                        zox_getn(tilemap),
                        zox_getv(tilemap, GenerateTexture));
                }
                continue;
            }
            tilemap_uvs = zox_get(tilemap, TilemapUVs);
#ifdef zox_safety_checks
            if (!tilemap_uvs->value || !tilemap_uvs->length) {
                zox_loge("Tilemap TilemapUVs on Chunk Terrain [%s] has not generated", zox_get_name(e));
                continue;
            }
#endif
            tilemap_cache = tilemap;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        float terrain_scale = zox_getv(terrain, BlockScale);
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        const SidesOctree* sides = zox_get(chunk, SidesOctree);
        float block_scale = get_chunk_scale(depth->value, terrain_depth, terrain_scale);
        short length = octree_size(depth->value);
        float chunk_scale = block_scale * length;
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
        };
        // read_lock_VoxelNode(voxels);
        zox_terrain_building_dig(
            data,
            sides,
            voxels,
            byte3_zero,
            chunk_scale,
            depth->value,
            0,
            dbg_log);
        // read_unlock_VoxelNode(voxels);
        // Set Entity data from Dynamic Arrays
        indicies->length = mesh_data.indicies->size;
        verts->length = mesh_data.vertices->size;
        uvs->length = mesh_data.uvs->size;
        colors->length = mesh_data.color_rgbs->size;
        indicies->value = finalize_arrayd_int(mesh_data.indicies);
        verts->value = finalize_arrayd_float3(mesh_data.vertices);
        colors->value = finalize_arrayd_color_rgb(mesh_data.color_rgbs);
        uvs->value = finalize_arrayd_float2(mesh_data.uvs);
        // dirty
        zox_remove(e, BuildMesh);
        if (!disable_lights) {
            zox_add(e, BuildMeshColors);
        }
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("Built Mesh [%s]:[%s] Verts [%i] Scale [%f] Depth [%i]",
                zox_getn(e),
                zox_getn(chunk),
                verts->length,
                chunk_scale,
                depth->value);
        }
        zox_sys_increment();
    }
} zox_sys_end(ChunkTexturedBuildSystem);


zox_sys2(ChunkMeshTestSystem) {
    // zox_log("ChunkMeshTestSystem [%i]", it->count);
    //char *iter_str = ecs_iter_str(it);
    // char *table_str = ecs_table_str(it->world, it->table);
    zox_log(
        "ChunkMeshTestSystem [%d] table=%p",
        it->count,
        (void*) it->table
        //iter_str,
        // table_str
    );
    // ecs_os_free(table_str);
    //ecs_os_free(iter_str);
} zox_sys_end(ChunkMeshTestSystem);