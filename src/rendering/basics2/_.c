
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "mat/_.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_materials_elements(ecs *world) {
    spawn_material_matrixui(world);
}

zox_begin_module(Rendering2) {
    zox_define_components_basics2(world);
    define_systems_basics2D(world);
    add_hook_load_shader(&spawn_shaders_basics2D);
    add_hook_load_shader(&spawn_materials_elements);
} zox_end_module(Rendering2);

