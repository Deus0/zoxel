#ifndef zoxm_terrain_collisions
#define zoxm_terrain_collisions

// #define zox_dbg_lines_unstuck

#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(TerrainsCollisions) {
    zox_systems_terrain_collisions(world);
} zox_end_module(TerrainsCollisions);

#endif
