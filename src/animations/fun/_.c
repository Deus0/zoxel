#include "eternal.c"
#include "lerp.c"

// Set entity to value over time
#define zox_lerp_float(e, T, v, t) spawn_lerp_float(world, e, zox_id(T), v, t)
