// A fixture: an all air chunk at full detail, in a world of its own (no player), streamed round a streamer at the origin
//  - for tests that build something in an empty chunk (place_mesh, water_block, water_flow, water_splash)
//  - register the test with zox_tst_set_setup(name, NULL, 1): it runs on the menus, no game started
//  - its realm: zox_tst_world_realm, its terrain: zox_tst_world_terrain
//  - block voxes spawn in it (vodes3/sys/spawn.c): place a block and wait for its voxel's link
entity zox_tst_air_streamer = 0;
entity zox_tst_air_found = 0;
double zox_tst_air_begin = 0;

// the chunk once streamed in and settled (a second after it's found, so its neighbours are there), 0 until then
entity zox_tst_streamed_air_chunk(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    if (!zox_tst_air_streamer) {
        zox_tst_air_streamer = spawn_streamer(world, prefab_streamer, terrain, float3_zero);
        zox_setv(zox_tst_air_streamer, StreamerLevel, 1);
        // at the origin its stream position hasn't moved: dirty, so it streams
        zox_setv(zox_tst_air_streamer, StreamDirty, zox_dirty_trigger);
        zox_setv(zox_tst_air_streamer, StreamDirty2, zox_dirty_trigger);
        zox_tst_air_begin = zox_current_time;
    }
    zox_tst_waiting = "an all air chunk at full detail";
    const byte depth = zox_getv(terrain, NodeDepth);
    if (!zox_tst_air_found) {
        for (int y = render_distance_y; y > 0 && !zox_tst_air_found; y--) {
            const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3) { 0, y, 0 });
            if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue) || zox_getv(chunk, NodeDepth) != depth ||
                (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)) || zox_has(chunk, ChunkUpdate))
            {
                continue;
            }
            const VoxelNode* voxels = zox_get(chunk, VoxelNode);
            if (!has_children_VoxelNode(voxels) && !voxels->value) {
                zox_tst_air_found = chunk;
                zox_tst_air_begin = zox_current_time;
            }
        }
        if (!zox_tst_air_found && zox_current_time - zox_tst_air_begin > 60) {
            for (int y = render_distance_y; y > 0; y--) {
                const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3) { 0, y, 0 });
                zox_log("[tst] air chunk: [0x%ix0] [%s] depth [%i] of [%i] generate [%i] voxels open [%i] value [%i]", y,
                    zox_valid(chunk) ? zox_getn(chunk) : "none", zox_valid(chunk) ? zox_getv(chunk, NodeDepth) : -1, depth,
                    zox_valid(chunk) && zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1,
                    zox_valid(chunk) ? has_children_VoxelNode(zox_get(chunk, VoxelNode)) : -1, zox_valid(chunk) ? zox_get(chunk, VoxelNode)->value : -1);
            }
            zox_loge("[tst] no all air chunk streamed in after 60s");
            zox_tst_air_begin = zox_current_time;
        }
        return 0;
    }
    return zox_current_time - zox_tst_air_begin > 1 ? zox_tst_air_found : 0;
}

// a realm block by its tag, i.e. zox_tst_air_block(world, zox_id(BlockWater))
static inline entity zox_tst_air_block(ecs* world, entity tag) {
    return zox_get_child_by_id(world, zox_tst_world_realm, tag);
}

// a block position local to the chunk, to a world position at its middle
static inline float3 zox_tst_air_position(ecs* world, entity chunk, float3 local) {
    const entity terrain = zox_tst_world_terrain;
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const float scale = zox_getv(terrain, BlockScale);
    const int3 c = zox_getv(chunk, ChunkPosition);
    return (float3) { (c.x * length + local.x + 0.5f) * scale, (c.y * length + local.y + 0.5f) * scale, (c.z * length + local.z + 0.5f) * scale };
}
