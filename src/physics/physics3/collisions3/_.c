
// todo: add AABB (then detailed voxel) to Sphere [overlap] events for pickups
// remember: no line detection for boxes, if velocity exceeds clipping it will fall through map!
#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_collisions3(ecs* world) {
    zox_module(collisions3);
    zox_components_collisions3(world);
    zox_systems_collisions3(world);
}