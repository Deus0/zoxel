#ifndef zoxm_dungeons
#define zoxm_dungeons

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void spawn_prefabs_dungeons(ecs* world) {
    add_hook_spawned_block(&spawn_world_dungeon_core);
}

zox_begin_module(Dungeons) {
    define_components_dungeons(world);
    define_systems_dungeons(world);
    add_hook_spawn_prefabs(spawn_prefabs_dungeons);
} zox_end_module(Dungeons);

#endif
