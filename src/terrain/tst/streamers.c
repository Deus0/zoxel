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

// tst: coop_streamers, local coop: three streamers near each other (2 chunks apart, their land overlapping), each
//  column's lod is its closest streamer's, the middle one moving doesn't fight the others' lods
//  - each streamer set the columns round it to its own distance when it moved, a closer streamer's land dropped
//      lods (and swapped back when that one moved): the lods fought, laggy, in split screen
//  - the middle one moves a chunk on and back each second for 6 seconds: the outer two's columns keep distance 0 and
//      their lod every frame
//  - settled: every column within the far lod of a streamer has its closest streamer's distance
//  - run: bash bsh/tst.sh coop_streamers
#define zox_tst_coop_streamers_count 3
#define zox_tst_coop_streamers_apart 2
byte zox_tst_coop_stage = 0;
double zox_tst_coop_time = 0;
entity zox_tst_coop_streamers[zox_tst_coop_streamers_count];
byte zox_tst_coop_depths[zox_tst_coop_streamers_count];
int zox_tst_coop_fights = 0;
int zox_tst_coop_lod_swaps = 0;
int zox_tst_coop_wrong = 0;
int zox_tst_coop_columns = 0;

static inline int2 zox_tst_coop_point(ecs* world, byte i) {
    const int3 p = zox_getv(zox_tst_coop_streamers[i], StreamPosition);
    return (int2) { p.x, p.z };
}

static inline entity zox_tst_coop_tunk(ecs* world, entity terrain, int2 position) {
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, position);
    return zox_valid(tunk) && zox_has(tunk, RenderDistance) ? tunk : 0;
}

// the columns round every streamer: how many have a distance other than to their closest streamer
static inline int zox_tst_coop_wrong_distances(ecs* world, entity terrain, int* columns) {
    int wrong = 0;
    *columns = 0;
    const int far = terrain_lod_far;
    for (int x = -far; x <= (zox_tst_coop_streamers_count - 1) * zox_tst_coop_streamers_apart + far; x++)
    for (int z = -far - 2; z <= far + 2; z++) {
        const int2 position = (int2) { x, z };
        int closest = 255;
        for (byte i = 0; i < zox_tst_coop_streamers_count; i++) {
            const int d = int2_distance(zox_tst_coop_point(world, i), position);
            closest = d < closest ? d : closest;
        }
        const entity tunk = closest <= far ? zox_tst_coop_tunk(world, terrain, position) : 0;
        if (!tunk) {
            continue;
        }
        (*columns)++;
        const int distance = zox_getv(tunk, RenderDistance);
        if (distance != closest) {
            if (wrong < 5) {
                zox_log("[tst] column [%ix%i] has distance [%i], its closest streamer is [%i] away", x, z, distance, closest);
            }
            wrong++;
        }
    }
    return wrong;
}

byte zox_tst_coop_streamers_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    const double t = zox_current_time - zox_tst_coop_time;
    zox_tst_progress(zox_tst_coop_stage * 100 + (int) t);
    if (zox_tst_coop_stage == 0) {
        for (byte i = 0; i < zox_tst_coop_streamers_count; i++) {
            zox_tst_coop_streamers[i] = spawn_streamer(world, prefab_streamer, terrain,
                (float3) { (i * zox_tst_coop_streamers_apart + 0.5f) * chunk, chunk, 0.5f * chunk });
            zox_setv(zox_tst_coop_streamers[i], StreamerLevel, 1);
        }
        zox_tst_coop_stage = 1;
        zox_tst_coop_time = zox_current_time;
        return 0;
    }
    if (zox_tst_coop_stage == 1) {
        zox_tst_waiting = "every streamer's land to load";
        for (byte i = 0; i < zox_tst_coop_streamers_count; i++) {
            if (zox_tst_streamers_loaded(world, terrain, zox_getv(zox_tst_coop_streamers[i], Position3D)) < 9) {
                return t > 60;
            }
        }
        for (byte i = 0; i < zox_tst_coop_streamers_count; i++) {
            const entity tunk = zox_tst_coop_tunk(world, terrain, zox_tst_coop_point(world, i));
            zox_tst_coop_depths[i] = tunk ? zox_getv(tunk, RenderDepth) : 0;
        }
        zox_tst_coop_stage = 2;
        zox_tst_coop_time = zox_current_time;
        return 0;
    }
    if (zox_tst_coop_stage == 2) {
        // the middle one a chunk on, then back, each second
        zox_tst_waiting = "the middle streamer moving, the others' lods kept";
        const entity middle = zox_tst_coop_streamers[1];
        const float3 position = zox_getv(middle, Position3D);
        const float z = ((int) t % 2 ? 1.5f : 0.5f) * chunk;
        if (position.z != z) {
            zox_setv(middle, Position3D, ((float3) { position.x, position.y, z }));
        }
        for (byte i = 0; i < zox_tst_coop_streamers_count; i += 2) {
            const entity tunk = zox_tst_coop_tunk(world, terrain, zox_tst_coop_point(world, i));
            zox_tst_coop_fights += !tunk || zox_getv(tunk, RenderDistance) != 0;
            zox_tst_coop_lod_swaps += !tunk || zox_getv(tunk, RenderDepth) != zox_tst_coop_depths[i];
        }
        if (t < 6) {
            return 0;
        }
        zox_tst_coop_stage = 3;
        zox_tst_coop_time = zox_current_time;
        return 0;
    }
    // settled: the queue empty a second
    zox_tst_waiting = "the stream queue to empty";
    if (zox_get(terrain, TerrainSpawnQueue)->count) {
        zox_tst_coop_time = zox_current_time;
        return 0;
    }
    if (t < 1) {
        return 0;
    }
    zox_tst_coop_wrong = zox_tst_coop_wrong_distances(world, terrain, &zox_tst_coop_columns);
    zox_tst_coop_stage = 4;
    return 1;
}

void zox_tst_coop_streamers_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] coop_streamers: frames an outer streamer's column wasn't at distance 0 [%i], had another lod [%i]; columns [%i], with a wrong distance [%i]",
        zox_tst_coop_fights, zox_tst_coop_lod_swaps, zox_tst_coop_columns, zox_tst_coop_wrong);
    zox_tst_check(zox_tst_coop_stage == 4, "the streamers' land never loaded and settled (stage [%i])", zox_tst_coop_stage);
    zox_tst_check(!zox_tst_coop_fights, "an outer streamer's own column took the middle one's distance [%i] frames", zox_tst_coop_fights);
    zox_tst_check(!zox_tst_coop_lod_swaps, "an outer streamer's own column changed lod [%i] frames", zox_tst_coop_lod_swaps);
    zox_tst_check(zox_tst_coop_columns > 0 && !zox_tst_coop_wrong, "[%i] of [%i] columns don't have their closest streamer's distance",
        zox_tst_coop_wrong, zox_tst_coop_columns);
}

void zox_tsts_streamers(ecs* world) {
    zox_tst_add("two_streamers", zox_tst_two_streamers_ready, zox_tst_two_streamers_check, 120);
    zox_tst_set_setup("two_streamers", NULL, 1);
    zox_tst_add("coop_streamers", zox_tst_coop_streamers_ready, zox_tst_coop_streamers_check, 150);
    zox_tst_set_setup("coop_streamers", NULL, 1);
}
