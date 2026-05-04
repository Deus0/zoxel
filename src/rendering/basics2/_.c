#ifndef zoxm_rendering_basics2
#define zoxm_rendering_basics2

#include "set/_.c"
#include "dat/_.c"
#include "mat/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Rendering2)
    add_hook_load_shader(&spawn_shaders_basics2D);
    zoxd(MaterialTextured2D);
    define_systems_basics2D(world);
zox_end_module(Rendering2)

#endif
