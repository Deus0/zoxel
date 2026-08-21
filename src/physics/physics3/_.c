
#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "collisions3/_.c"

void import_physics3(ecs* world) {
    zox_module(physics3);
    zox_define_components_physics3(world);
    zox_systems_physics3(world);
    zox_add_module(collisions3);
}