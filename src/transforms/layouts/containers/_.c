// Containers like groups of elements, Grids, Lists, etc
#include "com/_.c"
#include "sys/_.c"

void import_containers(ecs* world) {
    zox_module(containers);
    zox_components_containers(world);
    zox_systems_containers(world);
}
