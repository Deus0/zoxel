/*
 * Module: Physics
 *
 *      - Moves things around
 *      - An apple falls from a tree
 *
 * */
// TODO: Fix the shaking issues on ground
float zox_physics_max_delta_time = 0.1f;
#include "com/_.c"
#include "physics2/_.c"
#include "physics3/_.c"
#include "dbg/_.c"

void import_physics(ecs* world) {
    zox_module(physics);
    zox_components_physics(world);
    zox_add_module(physics2);
    zox_add_module(physics3);
}