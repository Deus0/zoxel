/*
 * +------------------------------------------------------------------+
 * | Zox Module: XR                                                   |
 * |                                                                  |
 * |  Eyes - VR - Hands - Environments - OpenXR                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  - XR is on (zox_xr_active, cameras/_.c) with a headset (zox_xr) or the simulator (--xr-sim); other modules
 *    branch on it at runtime, not #ifdef zox_xr
 *  - pre, ins: the xr camera (XrCamera), spawned at boot while XR is on
 *  - fun: shared by both: free roam (the sticks move and turn it, the headset turns the head on top), an eye's render
 *  - openxr (zox_xr, Android): the session, eyes, swapchains, inputs, a headset's frame (openxr/render.c)
 *  - sim: the simulator, both eyes side by side in the window, the head and sticks from the keyboard
 *  - shared maths in cameras (fun/projection_angles.c, free_roam.c), tested there (cameras/tst/xr.c); tst/cube.c is
 *    a standalone app (free roam among cubes), not part of the engine's build
 *  - plan: doc/plans/xr.md
 */
#ifdef zox_xr
#include "openxr/_.c"
#endif
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sim/_.c"
#ifdef zox_xr
#include "openxr/render.c"

void module_dispose_xr(ecs *world, void *ctx) {
    xr_end();
}
#endif
#include "tst/_.c"

void import_xr(ecs* world) {
    zox_module(xr);
    add_hook_terminal_command(process_arguments_xr);
    add_hook_spawn_prefabs(zox_spawn_prefabs_xr);
    add_hook_on_boot(xr_spawn_camera);
    zox_systems_xr_sim(world);
    zox_tsts_xr(world);
#ifdef zox_xr
    zox_log("[XR] Engaged");
    zox_xr_active = 1;
    zox_systems_openxr(world);
    zox_module_dispose(module_dispose_xr);
#endif
}
