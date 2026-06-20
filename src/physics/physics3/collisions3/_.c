#ifndef zoxm_collisions3
#define zoxm_collisions3

// todo: add AABB (then detailed voxel) to Sphere [overlap] events for pickups
// remember: no line detection for boxes, if velocity exceeds clipping it will fall through map!
#include "dat/settings.c"
#include "dat/axis.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Collisions3) {
    define_components_collisions3(world);
    define_systems_collisions3(world);
} zox_end_module(Collisions3);

#endif
