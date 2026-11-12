/*
 * +------------------------------------------------------------------+
 * | Zox Module: Core                                                 |
 * |  Generics - Events - Shared                                      |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "timing/_.c"
#include "sys/_.c"
#include "settings/_.c"
#include "events/_.c"
#include "tst/_.c"

void import_core(ecs* world) {
    zox_module(core);
    initialize_zox(world);
    zox_components_core(world);
    zox_systems_core(world);
    initialize_tst(world);
    zox_tsts_flecs(world);
    add_hook_spawn_prefabs(zox_prefabs_core);
    zox_add_module(timing);
    zox_add_module(settings);
    zox_add_module(events);
}
