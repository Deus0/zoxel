
#include "com/_.c"
#include "sys/_.c"

void import_names(ecs* world) {
    zox_module(names);
    zox_components_names(world);
    zox_systems_names(world);
}