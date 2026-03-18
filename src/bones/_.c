#if !defined(zoxm_bones) && defined(zoxm_transforms)
#define zoxm_bones

// #define zox_debug_bones
// #define zox_transparent_skeletons
byte disable_bone_rendering = 0;
#include "sha/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/test.c"

zox_begin_module(Bones)
    add_hook_load_shader(&spawn_shaders_bones);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    define_components_bones(world);
    define_systems_bones(world);

    spawn_setting_byte(world, "transparent mesh", 0);
zox_end_module(Bones)

#endif
