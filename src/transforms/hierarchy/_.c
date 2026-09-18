uint hierarhys_children_capacity = 254;
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_hierarchys(ecs* world) {
    zox_module(hierarchys);
    zox_components_hierarchy(world);
    zox_systems_hierarchy(world);
}