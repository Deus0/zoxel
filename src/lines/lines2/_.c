#ifndef zoxm_lines2
#define zoxm_lines2

// todo: lines_ui should be seperate
// todo: add a purely lines2D that doesn't rely on UI transforms
#include "com/_.c"
#include "shd/line2D.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Lines2D)
    add_hook_load_shader(&spawn_shader_line2D);
    add_hook_spawn_prefabs(spawn_prefabs_lines2D);
    define_components_lines2(world);
    define_systems_line2(world);
zox_end_module(Lines2D)

#endif
