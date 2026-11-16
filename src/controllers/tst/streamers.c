// tst: two_streamers, the terrain streams round every streamer, not just the last one to move
//  - after the player spawns, a second streamer (prefab_streamer, level 1) 20 chunks away, then moved a chunk a
//      second for 4 seconds
//  - both places have their chunks at full detail and generated, and the player's stay while the other moves
//      (a regression: terrain streamed only round the last streamer to move, the other's land unloaded)
//  - run: bash bsh/tst.sh two_streamers
byte zox_tst_streamers_stage = 0;
double zox_tst_streamers_time = 0;
entity zox_tst_streamers_second = 0;
float3 zox_tst_streamers_player;
int zox_tst_streamers_player_lost = 0;

// a place's chunk columns within a chunk of it: how many are generated at full detail
static inline int zox_tst_streamers_loaded(ecs* world, entity terrain, float3 position) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int3 center = real_position_to_chunk_position(position, octree_size(depth), zox_getv(terrain, BlockScale));
    int loaded = 0;
    for (int x = -1; x <= 1; x++) for (int z = -1; z <= 1; z++) {
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3) { center.x + x, 0, center.z + z });
        loaded += zox_valid(chunk) && zox_has(chunk, NodeDepth) && zox_getv(chunk, NodeDepth) == depth &&
            !(zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk));
    }
    return loaded;
}

byte zox_tst_two_streamers_ready(ecs* world) {
    if (!zox_tst_streamers_stage && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity terrain = local_terrain;
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    zox_tst_progress(zox_tst_streamers_stage * 100 + (int) (zox_current_time - zox_tst_streamers_time));
    if (zox_tst_streamers_stage == 0) {
        const entity character = zox_get_link(world, dbg_player, CharacterLink);
        zox_tst_streamers_player = zox_getv(character, Position3D);
        const float3 far = float3_add(zox_tst_streamers_player, (float3) { 20 * chunk, 0, 0 });
        zox_tst_streamers_second = spawn_streamer(world, prefab_streamer, terrain, far);
        zox_setv(zox_tst_streamers_second, StreamerLevel, 1);
        zox_tst_streamers_stage = 1;
        zox_tst_streamers_time = zox_current_time;
        return 0;
    }
    const float3 second = zox_getv(zox_tst_streamers_second, Position3D);
    if (zox_tst_streamers_stage == 1) {
        zox_tst_waiting = "the second streamer's land to load";
        if (zox_tst_streamers_loaded(world, terrain, second) < 9) {
            return zox_current_time - zox_tst_streamers_time > 60;
        }
        zox_tst_streamers_stage = 2;
        zox_tst_streamers_time = zox_current_time;
        return 0;
    }
    // moving the second a chunk a second; the player's land must stay
    zox_tst_waiting = "the second streamer moving, the player's land kept";
    const double t = zox_current_time - zox_tst_streamers_time;
    zox_setv(zox_tst_streamers_second, Position3D, float3_add(second, (float3) { 0, 0, (float) (t > 4 ? 0 : 0.0167 * chunk) }));
    if (zox_tst_streamers_loaded(world, terrain, zox_tst_streamers_player) < 9) {
        zox_tst_streamers_player_lost++;
    }
    return t > 6;
}

void zox_tst_two_streamers_check(ecs* world) {
    const entity terrain = local_terrain;
    const int player = zox_tst_streamers_loaded(world, terrain, zox_tst_streamers_player);
    const int second = zox_valid(zox_tst_streamers_second) ?
        zox_tst_streamers_loaded(world, terrain, zox_getv(zox_tst_streamers_second, Position3D)) : 0;
    zox_log("[tst] [metric] two_streamers loaded at full detail of 9: by the player [%i], by the second [%i]; frames the player's weren't [%i]",
        player, second, zox_tst_streamers_player_lost);
    zox_tst_check(zox_tst_streamers_stage == 2, "the second streamer's land never loaded");
    zox_tst_check(second == 9, "only [%i] of 9 columns by the second streamer are loaded", second);
    zox_tst_check(player == 9 && !zox_tst_streamers_player_lost, "the player's land unloaded while the other streamer moved ([%i] of 9, [%i] frames)",
        player, zox_tst_streamers_player_lost);
}
