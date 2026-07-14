#ifndef zoxm_transforms2
#define zoxm_transforms2

// TODO: Layout can just effect local position
//          - Then we can modify transform easier of ui
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Transforms2) {
    zox_define_components_transforms2(world);
    zox_define_systems_transforms2(world);
} zox_end_module(Transforms2);

#endif