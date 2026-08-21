//#define max_settings 256
#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_settings(ecs* world) {
    zox_module(settings);
    zox_define_components_settings(world);
    define_systems_settings(world);
    add_hook_spawn_prefabs(spawn_prefabs_settings);
}
