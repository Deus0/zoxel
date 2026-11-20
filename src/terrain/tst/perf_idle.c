// tst: perf_idle, idle rendering: frames a second drawing the game standing still, to compare the backends (gl, vulkan)
//  and cpu with gpu meshing (--gpu_meshing); was render_fps (2026-11-20)
//  - waits for the player, then for streaming to settle (zox_tst_perf_busy, checked only then: it walks every
//    chunk), then counts frames for zox_tst_perf_idle_span
//  - logs frames a second, the average and slowest frame, chunks and vox instances drawn a frame; checks it drew
//  - numbers: the real gpu, release, no vsync, no post processing (vulkan has none yet), the same land:
//      zox_tst_seed=666 bash bsh/tst.sh perf_idle --gpu --release --novsync --nopost --far 32 [--vulkan]
//  - zox_tst_timeout: far terrain takes minutes to stream
#define zox_tst_perf_idle_span 8000.0
double zox_tst_perf_idle_start = 0;
double zox_tst_perf_idle_last = 0;
double zox_tst_perf_idle_worst = 0;
int zox_tst_perf_idle_frames = 0;
int zox_tst_perf_idle_settled = 0;
uint zox_tst_perf_idle_chunks = 0;
uint zox_tst_perf_idle_instances = 0;

byte zox_tst_perf_idle_ready(ecs* world) {
    const double now = get_time_ms();
    if (!zox_tst_perf_idle_start) {
        // only before counting: a player respawning mid count (a fall, an npc) stopped it, one 14s "frame"
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_waiting = "streaming to settle";
        const int busy = zox_tst_perf_busy(world);
        zox_tst_progress(-busy);
        zox_tst_perf_idle_settled = busy ? 0 : zox_tst_perf_idle_settled + 1;
        if (zox_tst_perf_idle_settled < 30) {
            return 0;
        }
        zox_tst_perf_idle_start = now;
        zox_tst_perf_idle_last = now;
        zox_tst_perf_idle_chunks = zox_drawn_chunks;
        zox_tst_perf_idle_instances = zox_drawn_vox_instances;
        return 0;
    }
    const double frame = now - zox_tst_perf_idle_last;
    zox_tst_perf_idle_worst = frame > zox_tst_perf_idle_worst ? frame : zox_tst_perf_idle_worst;
    zox_tst_perf_idle_frames++;
    zox_tst_perf_idle_last = now;
    return now - zox_tst_perf_idle_start >= zox_tst_perf_idle_span;
}

void zox_tst_perf_idle_check(ecs* world) {
    (void) world;
    const int frames = zox_tst_perf_idle_frames;
    const double seconds = (zox_tst_perf_idle_last - zox_tst_perf_idle_start) / 1000.0;
    const double fps = frames / seconds;
    zox_log("[tst] [metric] perf_idle [%s] far [%i] fps [%.1f] frame [%.2fms] slowest [%.2fms] frames [%i] chunks [%i] instances [%i] gpu [%i]",
        render_backend == zox_render_backend_vulkan ? "vulkan" : "opengl", terrain_lod_far, fps, 1000.0 / fps,
        zox_tst_perf_idle_worst, frames, frames ? (zox_drawn_chunks - zox_tst_perf_idle_chunks) / frames : 0,
        frames ? (zox_drawn_vox_instances - zox_tst_perf_idle_instances) / frames : 0, using_gpu);
    zox_tst_check(frames > 0, "no frames drawn");
}
