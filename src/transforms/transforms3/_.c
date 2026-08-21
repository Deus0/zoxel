
uint transforms3_children_capacity = 64;
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Transforms3) {
    define_components_transforms3(world);
    zox_define_systems_transforms3(world);
} zox_end_module(Transforms3);
