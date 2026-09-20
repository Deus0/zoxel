/*
 * +------------------------------------------------------+
 * | Zox Module: Dungeons                                 |
 * |                                                      |
 * |  Rooms - Layouts - Encounters - Traps - Growth       |
 * |                                                      |
 * +------------------------------------------------------+
 */
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void zox_prefabs_dungeons(ecs* world) {
    prefab_dungeon = spawn_prefab_dungeon(world);
    // add_hook_spawned_block(&spawn_world_dungeon_core);
}

void import_dungeons(ecs* world) {
    zox_module(dungeons);
    zox_components_dungeons(world);
    zox_systems_dungeons(world);
    add_hook_spawn_prefabs(zox_prefabs_dungeons);
    add_hook_spawned_block(&spawn_world_dungeon_core);
}
