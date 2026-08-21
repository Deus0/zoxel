/*
 * +------------------------------------------------------------------+
 * | Zox Module: XR                                                   |
 * |                                                                  |
 * |  Eyes - VR - Hands - Environments - OpenXR                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#if !defined(zoxm_xr) && defined(zox_xr)
#define zoxm_xr

#include "pre/_.c"
#include "fun/_.c"

void module_dispose_xr(ecs *world, void *ctx) {
    xr_shutdown();
    egl_shutdown();
}

void import_xr(ecs* world) {
    zox_module(xr);
    // Placeholder
    zox_log("XR Module Engage!");
    zox_loge("XRXRXRXR Module Engage!");
    if (!egl_setup()) {
        zox_loge("[XR] EGL setup failed");
        SDL_Quit();
        return 1;
    }
    if (!xr_init()) {
        zox_loge("[XR] OpenXR initialization failed");
        egl_shutdown();
        SDL_Quit();
        return 1;
    }
    if (!xr_setup()) {
        zox_loge("[XR] OpenXR setup failed");
        xr_shutdown();
        egl_shutdown();
        SDL_Quit();
        return 1;
    }
    zox_loge("XRXRXRXR SSUCCESSSS!");
    add_hook_spawn_prefabs(zox_spawn_prefabs_xr);
    zox_module_dispose(module_dispose_xr);
}

#endif
