#if !defined(zoxm_bones) && defined(zoxm_transforms)
#define zoxm_bones

byte is_paint_skeletons = 0;
#include "sha/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

zox_begin_module(Bones) {
    define_components_bones(world);
    define_systems_bones(world);
    add_hook_load_shader(&spawn_shaders_bones);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    add_hook_spawn_prefabs(spawn_settings_bones);
    initialize_settings_bones(world);
} zox_end_module(Bones);

#endif
