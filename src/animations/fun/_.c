#include "eternal.c"
#include "lerp.c"

// Set entity to value over time
#define zox_lerp_float(e, T, v, t) spawn_lerp_float_old(world, e, zox_id(T), v, t)

#define zox_lerp_float_ins(e, T, from, to, t) spawn_lerp_float(world, e, zox_id(T), from, to, t)