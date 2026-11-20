// tst: perf_terrain, terrain alone: its loading, drawing and streaming, nothing else in the world to blur the numbers
//  - no player, npcs, hud or particles: a realm and terrain of its own (world_only.c), one streamer (level 1), one
//      camera looking over it from above the streamer
//  - load: from the streamer's spawn until every chunk is generated, lit, meshed and settled (zox_tst_terrain_busy)
//  - idle: 8 seconds standing still once loaded (idle rendering)
//  - stream: the streamer moves a chunk along x, 6 steps, each until it settles (stream remeshing)
//  - tst/perf_terrain.ppm: its view once loaded
//  - each step logs when each kind of work last ran (queued, generating, sides, meshes, lights, swaps): what held it up
//  - logs each phase's frames (frames.c: fps average and 1% low, frame times, frames under 60 and 30 fps), the load
//      and step times, and the chunk meshes (cpu and gpu) at the end; compare cpu and gpu meshing with --gpu_meshing
//  - numbers: zox_tst_seed=666 bash bsh/tst.sh perf_terrain --release --gpu --stress --far 8 [--vulkan] [--gpu_meshing]
#define zox_tst_perf_terrain_idle_ms 8000.0
#define zox_tst_perf_terrain_steps 6
#define zox_tst_perf_terrain_step_max 30.0
extern byte nohud;
extern char zox_screenshot_request[256];
byte zox_tst_perf_terrain_stage = 0;
entity zox_tst_perf_terrain_streamer = 0;
double zox_tst_perf_terrain_time = 0;      // when the phase or step began
int zox_tst_perf_terrain_settled = 0;      // frames in a row with nothing busy
int zox_tst_perf_terrain_step = 0;
double zox_tst_perf_terrain_steps_total = 0, zox_tst_perf_terrain_steps_fastest = 0, zox_tst_perf_terrain_steps_slowest = 0;
zox_tst_frames zox_tst_perf_terrain_frames;
// when each kind of work was last busy in a step (seconds into it): queued, generating, sides, meshes, lights, swaps
double zox_tst_perf_terrain_last[6];
static const char* zox_tst_perf_terrain_kinds[6] = { "queued", "generating", "sides", "meshes", "lights", "swaps" };

// settled: nothing busy for 3 frames in a row
static inline byte zox_tst_perf_terrain_settle(ecs* world, entity terrain) {
    const int busy = zox_tst_terrain_busy(world, terrain);
    const int kinds[6] = { zox_tst_perf_queued, zox_tst_perf_generating, zox_tst_perf_siding, zox_tst_perf_meshing,
        zox_tst_perf_lighting, zox_tst_perf_swapping };
    const double into = (get_time_ms() - zox_tst_perf_terrain_time) / 1000.0;
    for (byte k = 0; k < 6; k++) {
        if (kinds[k]) {
            zox_tst_perf_terrain_last[k] = into;
        }
    }
    zox_tst_progress(zox_tst_perf_terrain_stage * 1000000.0 + zox_tst_perf_terrain_step * 100000.0 + busy);
    zox_tst_perf_terrain_settled = busy ? 0 : zox_tst_perf_terrain_settled + 1;
    return zox_tst_perf_terrain_settled >= 3;
}

static inline void zox_tst_perf_terrain_camera(ecs* world, entity terrain) {
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    // high, looking down the land: near chunks below, far lods to the horizon
    const float3 eye = { 0, chunk * 3, -chunk * 2 };
    const float3 target = { 0, -chunk, chunk * 3 };
    spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, target), 70, viewport_grid_cell(0, 1, 1));
    nohud = 1;
    const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
    if (zox_valid(game_view)) {
        zox_setv(game_view, RenderDisabled, 1);
    }
}

byte zox_tst_perf_terrain_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const double now = get_time_ms();
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    if (zox_tst_perf_terrain_stage == 0) {
        zox_tst_perf_terrain_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_perf_terrain_streamer, StreamerLevel, 1);
        zox_tst_perf_terrain_camera(world, terrain);
        zox_tst_frames_begin(&zox_tst_perf_terrain_frames);
        zox_tst_perf_terrain_time = now;
        zox_tst_perf_terrain_stage = 1;
        return 0;
    }
    zox_tst_frames_tick(&zox_tst_perf_terrain_frames);
    if (zox_tst_perf_terrain_stage == 1) {
        zox_tst_waiting = "the terrain to load: generated, lit, meshed";
        // a frame or two before the streamer's first stream
        if (now - zox_tst_perf_terrain_time < 100 || !zox_tst_perf_terrain_settle(world, terrain)) {
            return 0;
        }
        zox_log("[tst] [metric] perf_terrain load: settled in [%.3fs]", (now - zox_tst_perf_terrain_time) / 1000.0);
        zox_tst_frames_log(&zox_tst_perf_terrain_frames, "perf_terrain load");
        zox_tst_frames_begin(&zox_tst_perf_terrain_frames);
        zox_tst_perf_terrain_time = now;
        zox_tst_perf_terrain_stage = 2;
        return 0;
    }
    if (zox_tst_perf_terrain_stage == 2) {
        zox_tst_waiting = "idle frames";
        zox_tst_progress(2000000.0 + zox_tst_perf_terrain_frames.count);
        if (now - zox_tst_perf_terrain_time < zox_tst_perf_terrain_idle_ms) {
            return 0;
        }
        zox_tst_frames_log(&zox_tst_perf_terrain_frames, "perf_terrain idle");
        // what it drew (tst/perf_terrain.ppm), taken as the stream begins
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("perf_terrain.ppm"));
        zox_tst_frames_begin(&zox_tst_perf_terrain_frames);
        zox_tst_perf_terrain_stage = 3;
        zox_tst_perf_terrain_step = -1;
    }
    // stream: a step, then wait for it to settle
    zox_tst_waiting = "a stream step to settle";
    if (zox_tst_perf_terrain_step >= 0) {
        const double elapsed = (now - zox_tst_perf_terrain_time) / 1000.0;
        if (elapsed > zox_tst_perf_terrain_step_max) {
            zox_tst_check(0, "step [%i] didn't settle in [%.0fs]: generating [%i] sides [%i] meshes [%i] lights [%i] swapping [%i] queued [%i]",
                zox_tst_perf_terrain_step, zox_tst_perf_terrain_step_max, zox_tst_perf_generating, zox_tst_perf_siding,
                zox_tst_perf_meshing, zox_tst_perf_lighting, zox_tst_perf_swapping, zox_tst_perf_queued);
            return 1;
        }
        // its stream moved (a frame after the streamer did) and nothing's busy
        if (elapsed < 0.1 || zox_getv(zox_tst_perf_terrain_streamer, StreamDirty) || !zox_tst_perf_terrain_settle(world, terrain)) {
            return 0;
        }
        // what held the step up: when each kind of work last ran
        char held[256];
        int length = 0;
        for (byte k = 0; k < 6; k++) {
            length += snprintf(held + length, sizeof(held) - length, " %s [%.2fs]", zox_tst_perf_terrain_kinds[k],
                zox_tst_perf_terrain_last[k]);
        }
        zox_log("[tst] [metric] perf_terrain stream step [%i] settled in [%.3fs], last busy:%s", zox_tst_perf_terrain_step,
            elapsed, held);
        zox_tst_perf_terrain_steps_total += elapsed;
        zox_tst_perf_terrain_steps_slowest = elapsed > zox_tst_perf_terrain_steps_slowest ? elapsed : zox_tst_perf_terrain_steps_slowest;
        zox_tst_perf_terrain_steps_fastest = !zox_tst_perf_terrain_steps_fastest || elapsed < zox_tst_perf_terrain_steps_fastest ?
            elapsed : zox_tst_perf_terrain_steps_fastest;
    }
    zox_tst_perf_terrain_step++;
    if (zox_tst_perf_terrain_step >= zox_tst_perf_terrain_steps) {
        char label[128];
        zox_tst_frames_label(label, sizeof(label));
        zox_log("[tst] [metric] perf_terrain stream %s: steps [%i] settle in seconds average [%.3f] fastest [%.3f] slowest [%.3f]",
            label, zox_tst_perf_terrain_steps, zox_tst_perf_terrain_steps_total / zox_tst_perf_terrain_steps,
            zox_tst_perf_terrain_steps_fastest, zox_tst_perf_terrain_steps_slowest);
        zox_tst_frames_log(&zox_tst_perf_terrain_frames, "perf_terrain stream");
        return 1;
    }
    float3 position = zox_getv(zox_tst_perf_terrain_streamer, Position3D);
    position.x += chunk;
    zox_setv(zox_tst_perf_terrain_streamer, Position3D, position);
    zox_tst_perf_terrain_time = now;
    zox_tst_perf_terrain_settled = 0;
    memset(zox_tst_perf_terrain_last, 0, sizeof(zox_tst_perf_terrain_last));
    return 0;
}

void zox_tst_perf_terrain_check(ecs* world) {
    int meshes = 0, gpu_meshes = 0;
    zox_each_begin(MeshRenderCount);
        if (!zox_has(e, ChunkMesh) || zox_has(e, Disabled)) {
            continue;
        }
        meshes++;
        gpu_meshes += zox_has(e, GpuMesh);
    zox_each_end();
    zox_log("[tst] [metric] perf_terrain: [%i] chunk meshes shown, [%i] on the gpu, [%u] chunks drawn in all",
        meshes, gpu_meshes, zox_drawn_chunks);
    zox_tst_check(meshes, "no chunk meshes");
    zox_tst_frames_dispose(&zox_tst_perf_terrain_frames);
}

void zox_tsts_perf_terrain(ecs* world) {
    (void) world;
    zox_tst_add("perf_terrain", zox_tst_perf_terrain_ready, zox_tst_perf_terrain_check, 600);
    zox_tst_set_setup("perf_terrain", NULL, 1);
}
