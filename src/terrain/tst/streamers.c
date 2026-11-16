// tst: two_streamers, the terrain streams round every streamer, not just the last one to move
//  - no player: a realm and terrain of its own (world_only.c), two streamers 20 chunks apart (spawn_streamer, level 1)
//  - both places load at full detail, then the second moves a chunk a second for 4 seconds: the first's land stays
//      loaded every frame (it would unload if the terrain streamed only round the last streamer to move)
//  - run: bash bsh/tst.sh two_streamers
byte zox_tst_streamers_stage = 0;
double zox_tst_streamers_time = 0;
entity zox_tst_streamers[2];
int zox_tst_streamers_first_lost = 0;

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
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    zox_tst_progress(zox_tst_streamers_stage * 100 + (int) (zox_current_time - zox_tst_streamers_time));
    if (zox_tst_streamers_stage == 0) {
        for (byte i = 0; i < 2; i++) {
            zox_tst_streamers[i] = spawn_streamer(world, prefab_streamer, terrain, (float3) { i * 20 * chunk, chunk, 0 });
            zox_setv(zox_tst_streamers[i], StreamerLevel, 1);
        }
        zox_tst_streamers_stage = 1;
        zox_tst_streamers_time = zox_current_time;
        return 0;
    }
    const float3 first = zox_getv(zox_tst_streamers[0], Position3D);
    const float3 second = zox_getv(zox_tst_streamers[1], Position3D);
    if (zox_tst_streamers_stage == 1) {
        zox_tst_waiting = "both streamers' land to load";
        if (zox_tst_streamers_loaded(world, terrain, first) < 9 || zox_tst_streamers_loaded(world, terrain, second) < 9) {
            return zox_current_time - zox_tst_streamers_time > 60;
        }
        zox_tst_streamers_stage = 2;
        zox_tst_streamers_time = zox_current_time;
        return 0;
    }
    // the second moving a chunk a second, the first's land must stay
    zox_tst_waiting = "the second streamer moving, the first's land kept";
    const double t = zox_current_time - zox_tst_streamers_time;
    zox_setv(zox_tst_streamers[1], Position3D, float3_add(second, (float3) { 0, 0, (float) (t > 4 ? 0 : 0.0167 * chunk) }));
    zox_tst_streamers_first_lost += zox_tst_streamers_loaded(world, terrain, first) < 9;
    return t > 6;
}

void zox_tst_two_streamers_check(ecs* world) {
    const entity terrain = zox_tst_world_terrain;
    zox_tst_check(zox_valid(terrain), "no terrain was made");
    if (!zox_valid(terrain)) {
        return;
    }
    const int first = zox_tst_streamers_loaded(world, terrain, zox_getv(zox_tst_streamers[0], Position3D));
    const int second = zox_tst_streamers_loaded(world, terrain, zox_getv(zox_tst_streamers[1], Position3D));
    zox_log("[tst] [metric] two_streamers loaded at full detail of 9: by the first [%i], by the second [%i]; frames the first's weren't [%i]",
        first, second, zox_tst_streamers_first_lost);
    zox_tst_check(zox_tst_streamers_stage == 2, "the streamers' land never loaded");
    zox_tst_check(second == 9, "only [%i] of 9 columns by the second streamer are loaded", second);
    zox_tst_check(first == 9 && !zox_tst_streamers_first_lost, "the first streamer's land unloaded while the second moved ([%i] of 9, [%i] frames)",
        first, zox_tst_streamers_first_lost);
}

void zox_tsts_streamers(ecs* world) {
    zox_tst_add("two_streamers", zox_tst_two_streamers_ready, zox_tst_two_streamers_check, 120);
    zox_tst_set_setup("two_streamers", NULL, 1);
}
