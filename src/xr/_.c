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
 *  - pre, ins: the xr camera (XrCamera) and controllers (XrController, a device a hand), spawned at boot while XR is on
 *  - the ui: huds spawns its menus as 3D lists (zox_huds3D), navigated and clicked by the controllers
 *  - fun: shared by both: the hands (the body's arms reach the controllers, bones/ik), free roam (the sticks move and turn it, the headset turns the head on top), an eye's render
 *  - openxr (zox_xr, Android): the session, eyes, swapchains, inputs, a headset's frame (openxr/render.c)
 *  - sim: the simulator, both eyes side by side in the window, the head and sticks from the keyboard
 *  - shared maths in cameras (fun/projection_angles.c, free_roam.c), tested there (cameras/tst/xr.c); tst/cube.c is
 *    a standalone app (free roam among cubes), not part of the engine's build
 *  - plan: doc/plans/xr.md
 */
#ifdef zox_xr
#include "openxr/_.c"
#endif
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sim/_.c"
#ifdef zox_xr
#include "openxr/controller.c"
#include "openxr/render.c"

void module_dispose_xr(ecs *world, void *ctx) {
    xr_end();
}
#endif
#include "tst/_.c"

// a headset draws two eyes at a high refresh rate, so its defaults are lighter than the desktop's (a saved
//  settings.cfg still wins): a shorter view, no grass animation, fewer npcs closer in
static inline void xr_lower_graphics_defaults() {
    terrain_lod_near = 2;
    terrain_lod_far = 4;
    render_distance_y = 2;
    zox_grass_animation = 0;
    character_spawn_rate_max = 2;
    character_spawn_distance = 1;
}

void import_xr(ecs* world) {
    zox_module(xr);
    add_hook_terminal_command(process_arguments_xr);
    zoxd_tag(XrController);
    zoxd_byte(XrHand);
    add_hook_spawn_prefabs(zox_spawn_prefabs_xr);
    add_hook_on_boot(xr_spawn_camera);
    add_hook_on_boot(xr_sim_feed_devices);
    zox_systems_xr_hands(world);
    zox_systems_xr_body(world);
    zox_systems_xr_scale(world);
    zox_systems_xr_sim(world);
    zox_tsts_xr(world);
#ifdef zox_xr
    zox_log("[XR] Engaged");
    zox_xr_active = 1;
    zox_huds3D = 1;
    xr_lower_graphics_defaults();
    zox_systems_openxr(world);
    zox_module_dispose(module_dispose_xr);
#endif
}
