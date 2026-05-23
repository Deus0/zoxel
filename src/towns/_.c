#ifndef zox_towns
#define zox_towns

#include "com/_.c"
#include "sys/_.c"

void zox_define_prefabs_towns(ecs* world) {
    zox_prefab_set(prefab_tunk2, TownMap, { 0 });
}

zox_begin_module(Towns) {
    zox_define_components_towns(world);
    zox_define_systems_towns(world);
    add_hook_spawn_prefabs(zox_define_prefabs_towns);
} zox_end_module(Towns)

#endif
