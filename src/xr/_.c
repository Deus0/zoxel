/*
 * +------------------------------------------------------------------+
 * | Zox Module: XR                                                   |
 * |                                                                  |
 * |  Eyes - VR - Hands - Environments - OpenXR                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#ifdef zox_xr

#include "_.h"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void module_dispose_xr(ecs *world, void *ctx) {
    xr_end();
}

void import_xr(ecs* world) {
    zox_module(xr);
    zox_module_dispose(module_dispose_xr);
    zox_components_xr(world);
    zox_systems_xr(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_xr);
}

#endif
