#ifndef zoxm_rendering_basics3
#define zoxm_rendering_basics3

#include "dat/_.c"
#include "mat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Rendering3) {
    zox_define_components_basics3(world);
    zox_define_systems_basics3(world);
    add_hook_load_shader(&spawn_shaders_basics3D);
} zox_end_module(Rendering3);

#endif
