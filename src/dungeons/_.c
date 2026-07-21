#ifndef zoxm_dungeons
#define zoxm_dungeons

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void zox_define_prefabs_dungeons(ecs* world) {
    prefab_dungeon = spawn_prefab_dungeon(world);
    // add_hook_spawned_block(&spawn_world_dungeon_core);
}

zox_begin_module(Dungeons) {
    define_components_dungeons(world);
    define_systems_dungeons(world);
    add_hook_spawned_block(&spawn_world_dungeon_core);
    add_hook_spawn_prefabs(zox_define_prefabs_dungeons);
} zox_end_module(Dungeons);

#endif
