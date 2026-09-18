/*
 * +------------------------------------------------------------------+
 * | Zox Module: NPCs                                                 |
 * |                                                                  |
 * |  Characters - Spawning - Placement - Schedules - NPC State       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#include "sys/_.c"

void import_npcs(ecs* world) {
    zox_module(npcs);
    zox_components_characters3_terrain(world);
    zox_systems_characters3_terrain(world);
    add_hook_spawn_prefabs(spawn_prefabs_characters3_terrain);
}
