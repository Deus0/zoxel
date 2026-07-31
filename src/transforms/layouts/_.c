// Anchor/Grid Aware Positioning for our UIs and 2D Games!
#if !defined(zoxm_layouts2) && defined(zoxm_transforms2)
#define zoxm_layouts2

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

zox_begin_module(Layouts) {
    define_components_layouts2(world);
    define_systems_layouts2(world);
    add_hook_spawn_prefabs(spawn_prefabs_layouts2);
    zox_import_module(Containers);
} zox_end_module(Layouts);

#endif
