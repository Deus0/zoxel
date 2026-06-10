/*
 *  Zox Towns
 *      - A Region has Many Towns
 *      - A town is a return point from adventures
 *
 *  Towns Have
 *      - Houses
 *      - Walls
 *      - Roads
 *      - NPCs
 *      - Respawn Points
 *
 * */
#ifndef zox_towns
#define zox_towns

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Towns) {
    zox_define_components_towns(world);
    zox_define_systems_towns(world);
    add_hook_spawn_prefabs(zox_define_prefabs_towns);
} zox_end_module(Towns);

#endif
