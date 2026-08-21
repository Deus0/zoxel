// Anchor/Grid Aware Positioning for our UIs and 2D Games!

float zox_depth_per_layer3 = 0; //  0.1f;
float zox_depth_per_layer = 0.001f;
uint layouts2_children_capacity = 64;  // for big ass text
#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "ins/_.c"
#include "containers/_.c"

void import_layouts(ecs* world) {
    zox_module(layouts);
    define_components_layouts2(world);
    define_systems_layouts2(world);
    add_hook_spawn_prefabs(spawn_prefabs_layouts2);
    zox_import_module(Containers);
}
