/*
 * Module: Physics
 *
 *      - Moves things around
 *      - An apple falls from a tree
 *
 * */
// TODO: Fix the shaking issues on ground
#ifndef zoxm_physics
#define zoxm_physics

float zox_physics_max_delta_time = 0.1f;
#include "com/_.c"
#include "physics2/_.c"
#include "physics3/_.c"
#include "dbg/_.c"

zox_begin_module(Physics) {
    zox_define_components_physics(world);
    zox_import_module(Physics2);
    zox_import_module(Physics3);
} zox_end_module(Physics);

#endif
