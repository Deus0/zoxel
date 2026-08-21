
#include "sha/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

zox_begin_module(Bones) {
    define_components_bones(world);
    define_systems_bones(world);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    add_hook_load_shader(&spawn_shaders_bones);
} zox_end_module(Bones);
