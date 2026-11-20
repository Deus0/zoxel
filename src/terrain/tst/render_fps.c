// tst: render_fps, frames a second drawing the game standing still, to compare the backends (gl, vulkan)
//  - waits for the player, then for streaming to settle (zox_tst_perf_busy, checked only then: it walks every
//    chunk), then counts frames for zox_tst_render_fps_span
//  - logs frames a second, the average and slowest frame, chunks and vox instances drawn a frame; checks it drew
//  - numbers: the real gpu, release, no vsync, no post processing (vulkan has none yet), the same land:
//      zox_tst_seed=666 bash bsh/tst.sh render_fps --gpu --release --novsync --nopost --far 32 [--vulkan]
//  - zox_tst_timeout: far terrain takes minutes to stream
#define zox_tst_render_fps_span 8000.0
double zox_tst_render_fps_start = 0;
double zox_tst_render_fps_last = 0;
double zox_tst_render_fps_worst = 0;
int zox_tst_render_fps_frames = 0;
int zox_tst_render_fps_settled = 0;
uint zox_tst_render_fps_chunks = 0;
uint zox_tst_render_fps_instances = 0;

byte zox_tst_render_fps_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const double now = get_time_ms();
    if (!zox_tst_render_fps_start) {
        zox_tst_waiting = "streaming to settle";
        const int busy = zox_tst_perf_busy(world);
        zox_tst_progress(-busy);
        zox_tst_render_fps_settled = busy ? 0 : zox_tst_render_fps_settled + 1;
        if (zox_tst_render_fps_settled < 30) {
            return 0;
        }
        zox_tst_render_fps_start = now;
        zox_tst_render_fps_last = now;
        zox_tst_render_fps_chunks = zox_drawn_chunks;
        zox_tst_render_fps_instances = zox_drawn_vox_instances;
        return 0;
    }
    const double frame = now - zox_tst_render_fps_last;
    zox_tst_render_fps_worst = frame > zox_tst_render_fps_worst ? frame : zox_tst_render_fps_worst;
    zox_tst_render_fps_frames++;
    zox_tst_render_fps_last = now;
    return now - zox_tst_render_fps_start >= zox_tst_render_fps_span;
}

void zox_tst_render_fps_check(ecs* world) {
    (void) world;
    const int frames = zox_tst_render_fps_frames;
    const double seconds = (zox_tst_render_fps_last - zox_tst_render_fps_start) / 1000.0;
    const double fps = frames / seconds;
    zox_log("[tst] [metric] render_fps [%s] far [%i] fps [%.1f] frame [%.2fms] slowest [%.2fms] frames [%i] chunks [%i] instances [%i] gpu [%i]",
        render_backend == zox_render_backend_vulkan ? "vulkan" : "opengl", terrain_lod_far, fps, 1000.0 / fps,
        zox_tst_render_fps_worst, frames, frames ? (zox_drawn_chunks - zox_tst_render_fps_chunks) / frames : 0,
        frames ? (zox_drawn_vox_instances - zox_tst_render_fps_instances) / frames : 0, using_gpu);
    zox_tst_check(frames > 0, "no frames drawn");
}
