// tst: perf_stream, measures streaming as the player moves across chunks, and the world's size
//  - after the player spawns, moves its character one chunk at a time in a straight line
//  - times each step until the terrain settles: streaming moved to the new chunk, and no chunks
//      are generating, building sides or meshes
//  - logs [tst] [metric] lines (step times, flecs tables) to compare runs, i.e. when optimizing
//      (software rendering in tests, so compare runs on the same machine)
//  - fails if a step never settles (streaming stuck)
//  - run: bash bsh/tst.sh perf_stream
#define zox_tst_perf_steps 6
#define zox_tst_perf_step_max_seconds 20.0
int zox_tst_perf_step = -1;     // -1 waiting for the player to spawn
double zox_tst_perf_step_time = 0;
double zox_tst_perf_total = 0;
double zox_tst_perf_slowest = 0;
int zox_tst_perf_settled_frames = 0;

// the terrain's chunks still generating or building sides, and their meshes still building
//  (counting the tags world wide also counts prefabs and other voxel chunks, i.e. characters)
int zox_tst_perf_generating = 0;
int zox_tst_perf_siding = 0;
int zox_tst_perf_meshing = 0;
static inline int zox_tst_perf_busy(ecs* world) {
    zox_tst_perf_generating = 0;
    zox_tst_perf_siding = 0;
    zox_tst_perf_meshing = 0;
    if (!zox_valid(local_terrain)) {
        return 0;
    }
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            if (!zox_has(chunk, Chunk3)) {
                continue;
            }
            zox_tst_perf_generating += zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk);
            zox_tst_perf_siding += zox_has(chunk, BuildChunkSides);
            iter it3 = zox_children(world, chunk);
            while (zox_children_next(it3)) {
                for (int k = 0; k < it3.count; k++) {
                    zox_tst_perf_meshing += zox_has(it3.entities[k], ChunkMesh) && zox_has(it3.entities[k], BuildMesh);
                }
            }
        }
    }
    return zox_tst_perf_generating + zox_tst_perf_siding + zox_tst_perf_meshing;
}

byte zox_tst_perf_stream_ready(ecs* world) {
    if (zox_tst_perf_step == -1) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_log("[tst] [metric] perf_stream spawn_tables [%i]", zox_get_tables_count(world));
        zox_tst_perf_step = 0;
        zox_tst_perf_step_time = 0;
    }
    zox_tst_waiting = "streaming to settle after a step";
    const int busy = zox_tst_perf_busy(world);
    zox_tst_progress(zox_tst_perf_step * 100000.0 + busy);
    if (zox_tst_perf_step_time) {
        const double elapsed = zox_current_time - zox_tst_perf_step_time;
        // streaming noticed the move: the camera streams from the chunk it's in
        byte streamed = 0;
        const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
        if (zox_valid(camera) && zox_has(camera, StreamPosition) && zox_valid(local_terrain)) {
            const float scale = zox_getv(local_terrain, BlockScale);
            const int length = octree_size(zox_getv(local_terrain, NodeDepth));
            const int3 camera_chunk = real_position_to_chunk_position(zox_getv(camera, Position3D), length, scale);
            const int3 stream = zox_getv(camera, StreamPosition);
            streamed = camera_chunk.x == stream.x && camera_chunk.z == stream.z && !zox_getv(camera, StreamDirty);
        }
        // settled for a few frames in a row
        zox_tst_perf_settled_frames = busy || !streamed ? 0 : zox_tst_perf_settled_frames + 1;
        if (zox_tst_perf_settled_frames < 3) {
            if (elapsed > zox_tst_perf_step_max_seconds) {
                zox_tst_check(0, "step [%i] didn't settle in [%.0fs], [%i] still busy: generating [%i] sides [%i] meshes [%i] (build disabled [%i])",
                    zox_tst_perf_step, zox_tst_perf_step_max_seconds, busy,
                    zox_tst_perf_generating, zox_tst_perf_siding, zox_tst_perf_meshing, zox_count_types(BuildDisabled));
                return 1;
            }
            return 0;
        }
        zox_log("[tst] [metric] perf_stream step [%i] settled in [%.3fs]", zox_tst_perf_step, elapsed);
        zox_tst_perf_total += elapsed;
        if (elapsed > zox_tst_perf_slowest) {
            zox_tst_perf_slowest = elapsed;
        }
        zox_tst_perf_step++;
        if (zox_tst_perf_step >= zox_tst_perf_steps) {
            zox_log("[tst] [metric] perf_stream steps [%i] average [%.3fs] slowest [%.3fs] tables [%i]",
                zox_tst_perf_steps, zox_tst_perf_total / zox_tst_perf_steps, zox_tst_perf_slowest, zox_get_tables_count(world));
            return 1;
        }
    }
    // moves the character one chunk along x
    const entity character = zox_tst_spawn_character(world);
    const entity terrain = local_terrain;
    if (!character || !zox_valid(terrain)) {
        zox_tst_check(0, "the player's character or terrain is gone");
        return 1;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const float chunk_length = octree_size(zox_getv(terrain, NodeDepth)) * scale;
    float3 position = zox_getv(character, Position3D);
    position.x += chunk_length;
    position.y += scale * 2;
    zox_setv(character, Position3D, position);
    zox_tst_perf_step_time = zox_current_time;
    zox_tst_perf_settled_frames = 0;
    return 0;
}

void zox_tst_perf_stream_check(ecs* world) {
    (void) world;
}
