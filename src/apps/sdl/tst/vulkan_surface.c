// tst: sdl_vulkan_surface, the window's surface lost and found again, as android does in the background (vulkan)
//  - frames drawn, the surface let go of (zox_sdl_vulkan_lost): no frames while it's gone; a new one made
//    (zox_sdl_vulkan_found): frames again, no validation errors (it came back black on android, 2026-11-20)
//  - needs a window's surface: bash bsh/tst.sh sdl_vulkan_surface --vulkan --visible (offscreen runs skip it)
#ifdef zox_vulkan
byte zox_tst_svs_stage = 0;
ulong zox_tst_svs_frames = 0;
ulong zox_tst_svs_lost_frames = 0;
int zox_tst_svs_waited = 0;

byte zox_tst_sdl_vulkan_surface_ready(ecs* world) {
    if (vk_frames_presented < 3) {
        return 0;
    }
    if (!vk_surface && !vk_surface_lost) {
        return 1;   // offscreen, nothing to lose
    }
    zox_tst_svs_waited++;
    if (zox_tst_svs_stage == 0) {
        zox_sdl_vulkan_lost();
        zox_tst_svs_frames = vk_frames_presented;
        zox_tst_svs_stage = 1;
        zox_tst_svs_waited = 0;
    } else if (zox_tst_svs_stage == 1 && zox_tst_svs_waited >= 10) {
        zox_tst_svs_lost_frames = vk_frames_presented - zox_tst_svs_frames;
        zox_sdl_vulkan_found(zox_getv(main_app, SDLWindow));
        zox_tst_svs_frames = vk_frames_presented;
        zox_tst_svs_stage = 2;
        zox_tst_svs_waited = 0;
    } else if (zox_tst_svs_stage == 2 && zox_tst_svs_waited >= 10) {
        return 1;
    }
    return 0;
}

void zox_tst_sdl_vulkan_surface_check(ecs* world) {
    (void) world;
    if (zox_tst_svs_stage == 0) {
        zox_log("[tst] no window surface (offscreen), run with --visible");
        return;
    }
    const ulong after = vk_frames_presented - zox_tst_svs_frames;
    zox_log("[tst] frames while lost [%lu], after found [%lu]", zox_tst_svs_lost_frames, after);
    zox_tst_check(!zox_tst_svs_lost_frames, "[%lu] frames drawn with no surface", zox_tst_svs_lost_frames);
    zox_tst_check(after >= 5, "only [%lu] frames after the surface came back", after);
    zox_tst_check(vk_surface && vk_swapchain, "no surface or swapchain after coming back");
    zox_tst_check(!vk_validation_errors, "[%i] validation errors", vk_validation_errors);
}
#endif
