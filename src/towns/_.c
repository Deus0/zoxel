/*
 *  Zox Towns
 *      - A Region has Many Towns
 *      - A town is a return point from adventures
 *
 *  Towns Have
 *      - Houses
 *      - Walls
 *      - Roads, street lamps
 *      - Highways between them (fun/highways.c)
 *      - NPCs
 *      - Respawn Points
 *
 * */
byte zox_disable_towns = 0;
byte zox_disable_homes = 0;
#include "com/_.c"
#include "pre/_.c"
#include "fun/build.c"
#include "fun/home.c"
#include "fun/paths.c"
#include "fun/town.c"
#include "fun/highways.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

#ifdef zox_tests
// the test level (_/tst zox_tst_add_level): no towns
void zox_tst_level_towns() {
    zox_disable_towns = 1;
}
#endif

void import_towns(ecs* world) {
    zox_module(towns);
    zox_components_towns(world);
    zox_systems_towns(world);
    add_hook_spawn_prefabs(zox_define_prefabs_towns);
    zox_systems_towns_debug(world);
#ifdef zox_tests
    zox_tsts_towns(world);
#endif
#ifdef zox_tests
    zox_tst_add_level(zox_tst_level_towns);
#endif
}
