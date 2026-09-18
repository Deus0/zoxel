
// todo: lines_ui should be seperate
// todo: add a purely lines2D that doesn't rely on UI transforms
#include "com/_.c"
#include "shd/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_lines2(ecs* world) {
    zox_module(lines2);
    add_hook_load_shader(&spawn_shader_line2D);
    add_hook_spawn_prefabs(spawn_prefabs_lines2D);
    zox_components_lines2(world);
    zox_systems_line2(world);
}
