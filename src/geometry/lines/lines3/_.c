
// should live for .1 seconds
const float line_frame_timing = 1 / 10.0f;
#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "shd/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_lines3(ecs* world) {
    zox_module(lines3);
    add_hook_load_shader(&spawn_shaders_lines3D);
    add_hook_spawn_prefabs(spawn_prefabs_lines3D);
    zox_components_lines3(world);
    zox_systems_lines3(world);
}
