// tst: render_fps, frames a second drawing the game standing still, to compare the backends (gl, vulkan)
//  - waits for the player, then zox_tst_render_fps_warmup for the land to stream in, then counts frames
//  - logs frames a second, the average and slowest frame; checks only that it drew
//  - numbers: the real gpu, release, no vsync, no post processing (vulkan has none yet), the same land:
//      zox_tst_seed=666 bash bsh/tst.sh render_fps --gpu --release --novsync --nopost [--vulkan]
#define zox_tst_render_fps_warmup 4000.0
#define zox_tst_render_fps_span 6000.0
double zox_tst_render_fps_start = 0;
double zox_tst_render_fps_last = 0;
double zox_tst_render_fps_worst = 0;
int zox_tst_render_fps_frames = 0;

byte zox_tst_render_fps_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const double now = get_time_ms();
    if (!zox_tst_render_fps_start) {
        zox_tst_render_fps_start = now + zox_tst_render_fps_warmup;
    }
    if (now >= zox_tst_render_fps_start) {
        const double frame = now - zox_tst_render_fps_last;
        zox_tst_render_fps_worst = frame > zox_tst_render_fps_worst ? frame : zox_tst_render_fps_worst;
        zox_tst_render_fps_frames++;
    }
    zox_tst_render_fps_last = now;
    return now - zox_tst_render_fps_start >= zox_tst_render_fps_span;
}

void zox_tst_render_fps_check(ecs* world) {
    (void) world;
    const double seconds = (zox_tst_render_fps_last - zox_tst_render_fps_start) / 1000.0;
    const double fps = zox_tst_render_fps_frames / seconds;
    zox_log("[tst] [metric] render_fps [%s] fps [%.1f] frame [%.2fms] slowest [%.2fms] frames [%i] gpu [%i]",
        render_backend == zox_render_backend_vulkan ? "vulkan" : "opengl", fps, 1000.0 / fps,
        zox_tst_render_fps_worst, zox_tst_render_fps_frames, using_gpu);
    zox_tst_check(zox_tst_render_fps_frames > 0, "no frames drawn");
}
