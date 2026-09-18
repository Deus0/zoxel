
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "mat/_.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_materials_elements(ecs *world) {
    spawn_material_matrixui(world);
}

void import_rendering2(ecs* world) {
    zox_module(rendering2);
    zox_components_basics2(world);
    zox_systems_basics2D(world);
    add_hook_load_shader(&spawn_shaders_basics2D);
    add_hook_load_shader(&spawn_materials_elements);
}
