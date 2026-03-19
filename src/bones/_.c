#if !defined(zoxm_bones) && defined(zoxm_transforms)
#define zoxm_bones

#include "sha/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/test.c"

zox_begin_module(Bones)
    add_hook_load_shader(&spawn_shaders_bones);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    add_hook_spawn_prefabs(spawn_settings_bones);
    define_components_bones(world);
    define_systems_bones(world);
    initialize_settings_bones(world);
zox_end_module(Bones)

#endif
