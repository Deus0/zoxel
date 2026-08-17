#if !defined(zoxm_xr) && defined(zox_xr)
#define zoxm_xr

#include "fun/_.c"

void module_dispose_xr(ecs *world, void *ctx) {
    zox_xr_shutdown();
}

zox_begin_module(Xr) {
    // Placeholder
    zox_log("XR Module Engage!");
    zox_loge("XRXRXRXR Module Engage!");
    if (!zox_xr_init()) {
        zox_loge("XR initialization failed!");
        return;
    }
    zox_loge("XRXRXRXR SSUCCESSSS!");
    zox_module_dispose(module_dispose_xr);
} zox_end_module(Xr);

#endif
