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
#if !defined(zoxm_controllers) && defined(zoxm_characters) && defined(zoxm_inputs) && defined(zoxm_physics)
#define zoxm_controllers

#include "dat/settings.c"
#include "sys/_.c"
#include "controllers2/_.c"
#include "controllers3/_.c"
#include "free_roam/_.c"
#include "dbg/_.c"

zox_begin_module(Controllers) {
    zox_define_systems_controllers(world);
    zox_import_module(Controllers2);
    zox_import_module(Controllers3);
    zox_import_module(FreeRoam);
} zox_end_module(Controllers);

#endif
