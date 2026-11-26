// tst: stream_unload, land the stream leaves is unloaded a little a frame, and kept if it comes back first
//  (src/terrains/voxel/sys/stream_unload.c)
//  - a world of its own, a streamer at the origin; once loaded, unloading is paused (zox_stream_unload_chunks 0)
//  - kept: the streamer steps a chunk along x, columns behind it are left (TunkUnloading); it steps back before they're
//      unloaded: a left column is the same tunk again, untagged
//  - a little a frame: unloading on, the streamer jumps far away: every old column is tagged, they go over frames,
//      no frame deleting more than zox_stream_unload_chunks and a column's stack, till none are left
//  - run: bash bsh/tst.sh stream_unload
byte zox_tst_unload_stage = 0;
entity zox_tst_unload_streamer = 0;
entity zox_tst_unload_tunk = 0;     // a column left, then come back to
int2 zox_tst_unload_tunk_position;
int zox_tst_unload_settled = 0;
int zox_tst_unload_frames = 0;
int zox_tst_unload_chunks_last = 0;
int zox_tst_unload_most = 0;        // the most chunks one frame deleted
int zox_tst_unload_stack = 0;

static inline entity zox_tst_unload_tunk_at(ecs* world, entity terrain, int2 position) {
    return int2_hashmap_get(zox_get(terrain, TunkLinks)->value, position);
}

// a column tagged TunkUnloading, 0 if none
static inline entity zox_tst_unload_find(ecs* world) {
    entity found = 0;
    zox_each_begin(TunkPosition);
        if (!found && zox_has(e, TunkUnloading)) {
            found = e;
        }
    zox_each_end();
    return found;
}

static inline byte zox_tst_unload_settle(ecs* world, entity terrain) {
    zox_tst_unload_settled = zox_tst_terrain_busy(world, terrain) ? 0 : zox_tst_unload_settled + 1;
    return zox_tst_unload_settled >= 3;
}

byte zox_tst_stream_unload_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    const int far_distance = terrain_lod_far;
    zox_tst_progress(zox_tst_unload_stage * 1000 + zox_tst_unload_frames);
    zox_tst_unload_frames++;
    if (zox_tst_unload_stage == 0) {
        zox_tst_unload_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_unload_streamer, StreamerLevel, 1);
        zox_tst_unload_stage = 1;
        return 0;
    }
    if (zox_tst_unload_stage == 1) {
        zox_tst_waiting = "the land to load";
        if (zox_tst_unload_frames < 10 || !zox_tst_unload_settle(world, terrain)) {
            return 0;
        }
        // paused, and a step along x: columns behind it are left
        zox_stream_unload_chunks = 0;
        float3 position = zox_getv(zox_tst_unload_streamer, Position3D);
        position.x += chunk;
        zox_setv(zox_tst_unload_streamer, Position3D, position);
        zox_tst_unload_stage = 2;
        zox_tst_unload_frames = 0;
        return 0;
    }
    if (zox_tst_unload_stage == 2) {
        zox_tst_waiting = "a column left (TunkUnloading)";
        zox_tst_unload_tunk = zox_tst_unload_find(world);
        if (!zox_tst_unload_tunk) {
            if (zox_tst_unload_frames > 600) {
                zox_tst_check(0, "no column was tagged TunkUnloading after the streamer stepped away");
                return 1;
            }
            return 0;
        }
        zox_tst_unload_tunk_position = zox_getv(zox_tst_unload_tunk, TunkPosition);
        zox_tst_unload_stack = zox_get(zox_tst_unload_tunk, Chunk3Stack)->length;
        // back before it's unloaded
        float3 position = zox_getv(zox_tst_unload_streamer, Position3D);
        position.x -= chunk;
        zox_setv(zox_tst_unload_streamer, Position3D, position);
        zox_tst_unload_stage = 3;
        zox_tst_unload_settled = 0;
        zox_tst_unload_frames = 0;
        return 0;
    }
    if (zox_tst_unload_stage == 3) {
        zox_tst_waiting = "the streamer back, the column kept";
        if (zox_tst_unload_frames < 10 || !zox_tst_unload_settle(world, terrain)) {
            return 0;
        }
        const entity tunk = zox_tst_unload_tunk_at(world, terrain, zox_tst_unload_tunk_position);
        zox_tst_check(tunk == zox_tst_unload_tunk, "the column the streamer came back to was made again, not kept");
        zox_tst_check(zox_valid(tunk) && !zox_has(tunk, TunkUnloading), "the column it came back to is still unloading");
        // on again, and far away: every old column goes, a little a frame
        zox_stream_unload_chunks = 64;
        float3 position = zox_getv(zox_tst_unload_streamer, Position3D);
        position.x += chunk * (far_distance * 2 + 4);
        zox_setv(zox_tst_unload_streamer, Position3D, position);
        zox_tst_unload_chunks_last = zox_count_types(Chunk3);
        zox_tst_unload_stage = 4;
        zox_tst_unload_frames = 0;
        return 0;
    }
    // chunks deleted this frame: those gone (the new place's spawning adds some, it can only hide deletes)
    zox_tst_waiting = "the old land unloaded";
    const int chunks = zox_count_types(Chunk3);
    const int gone = zox_tst_unload_chunks_last - chunks;
    zox_tst_unload_most = gone > zox_tst_unload_most ? gone : zox_tst_unload_most;
    zox_tst_unload_chunks_last = chunks;
    const int unloading = zox_count_types(TunkUnloading);
    if (zox_tst_unload_frames < 10 || unloading) {
        return zox_tst_unload_frames > 1200;
    }
    return 1;
}

void zox_tst_stream_unload_check(ecs* world) {
    (void) world;
    zox_tst_check(!zox_count_types(TunkUnloading), "[%i] columns still unloading", zox_count_types(TunkUnloading));
    zox_tst_check(zox_tst_unload_most <= 64 + zox_tst_unload_stack, "a frame deleted [%i] chunks, over [%i] and a stack [%i]",
        zox_tst_unload_most, 64, zox_tst_unload_stack);
    zox_log("[tst] stream_unload: the most chunks one frame deleted [%i] (budget [64], a stack [%i]), took [%i] frames",
        zox_tst_unload_most, zox_tst_unload_stack, zox_tst_unload_frames);
    zox_stream_unload_chunks = 64;
}

void zox_tsts_stream_unload(ecs* world) {
    (void) world;
    zox_tst_add("stream_unload", zox_tst_stream_unload_ready, zox_tst_stream_unload_check, 120);
    zox_tst_set_setup("stream_unload", NULL, 1);
}
