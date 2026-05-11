/*
 *  Module: Stats
 *
 *  - Health
 *  - Resources
 *  - Base Stats
 *  - Attributes
 *
 * */
#if !defined(zoxm_stats) && defined(zoxm_users)
#define zoxm_stats

uint stats_children_capacity = 64;

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "ui/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Stats)
    define_components_stats(world);
    define_systems_stats(world);
    add_hook_spawn_prefabs(spawn_prefabs_stats);
    // link to characters
    // add_hook_spawned_character3D(&spawn_character_stats);
    zox_import_module(StatsUI);
zox_end_module(Stats)

#endif
