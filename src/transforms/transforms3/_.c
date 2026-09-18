
uint transforms3_children_capacity = 251;
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_transforms3(ecs* world) {
    zox_module(transforms3);
    zox_components_transforms3(world);
    zox_systems_transforms3(world);
}
