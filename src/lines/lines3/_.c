#ifndef zoxm_lines3
#define zoxm_lines3

// should live for .1 seconds
const float line_frame_timing = 1 / 10.0f;
#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "shd/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Lines3) {
    add_hook_load_shader(&spawn_shaders_lines3D);
    add_hook_spawn_prefabs(spawn_prefabs_lines3D);
    define_components_lines3(world);
    define_systems_lines3(world);
} zox_end_module(Lines3);

#endif
