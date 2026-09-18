/*
 * +------------------------------------------------------------------+
 * | Zox Module: Controllers                                          |
 * |                                                                  |
 * |  Movement - Cameras - Input Binding - Behaviors - Control State  |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  USES:
 *
 *      - Characters
 *      - Cameras
 *      - UIs
 *
 * */
#include "dat/settings.c"
#include "sys/_.c"
#include "controllers2/_.c"
#include "controllers3/_.c"
#include "free_roam/_.c"
#include "dbg/_.c"

void import_controllers(ecs* world) {
    zox_module(controllers);
    zox_systems_controllers(world);
    zox_add_module(controllers2);
    zox_add_module(controllers3);
    zox_add_module(freeroam);
}