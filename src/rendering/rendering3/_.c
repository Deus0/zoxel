
#include "dat/_.c"
#include "mat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_rendering3(ecs* world) {
    zox_module(rendering3);
    zox_components_rendering3(world);
    zox_define_systems_basics3(world);
    add_hook_load_shader(&spawn_shaders_basics3D);
    add_hook_spawn_prefabs(&zox_prefabs_rendering3);
}

