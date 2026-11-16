/*
 * +------------------------------------------------------------------+
 * | Zox Module: XR                                                   |
 * |                                                                  |
 * |  Eyes - VR - Hands - Environments - OpenXR                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  - OpenXR (zox_xr, Android): the session, eyes, swapchains, inputs, the render; what isn't OpenXR's goes in
 *    the modules it belongs to (cameras/fun projection_angles.c, free_roam.c), built and tested everywhere
 *  - tests of its shared parts: cameras/tst/xr.c; tst/cube.c is a standalone app (free roam among cubes), not part of the engine's build
 */
#ifdef zox_xr
#include "_.h"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void module_dispose_xr(ecs *world, void *ctx) {
    xr_end();
}
#endif

void import_xr(ecs* world) {
    zox_module(xr);
#ifdef zox_xr
    zox_log("[XR] Engaged");
    zox_components_xr(world);
    zox_systems_xr(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_xr);
    add_hook_on_boot(xr_spawn_camera);
    zox_module_dispose(module_dispose_xr);
#endif
}
