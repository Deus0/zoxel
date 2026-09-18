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
byte zox_disable_towns = 0;
byte zox_disable_homes = 0;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_towns(ecs* world) {
    zox_module(towns);
    zox_components_towns(world);
    zox_systems_towns(world);
    add_hook_spawn_prefabs(zox_define_prefabs_towns);
    zox_systems_towns_debug(world);
}
