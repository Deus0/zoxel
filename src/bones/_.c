
#include "sha/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

void import_bones(ecs* world) {
    zox_module(bones);
    define_components_bones(world);
    define_systems_bones(world);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
}