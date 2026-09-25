/*
 * +------------------------------------------------------+
 * | Zox Module: Stats                                    |
 * |                                                      |
 * |  Health - Resources - Base Stats - Attributes        |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Implement Regen Stats (currently just using set rate)
 *
 */
float  combat_regen_mul = 0.25f;
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "ui/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "eve/_.c"

void import_stats(ecs* world) {
    zox_module(stats);
    zox_components_stats(world);
    zox_systems_stats(world);
    add_hook_spawn_prefabs(spawn_prefabs_stats);
    add_hook_spawn_prefabs(zox_events_stats);
    zox_add_module(statsui);
}
