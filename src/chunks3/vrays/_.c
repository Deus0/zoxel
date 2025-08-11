// VoxelRays into your heart <3
#ifndef zoxm_vrays
#define zoxm_vrays

const uint safety_checks_raycasting = 512;
const float raycast_thickness = 16;
color hit_terrain_color = { 2, 145, 145, 55 };
color hit_character_color = { 155, 45, 45, 65 };
color hit_block_vox_color = { 55, 185, 145, 45 };

#include "components/_.c"
#include "util/_.c"
#include "systems/_.c"
#include "debug/_.c"

zox_begin_module(Vrays)
    define_components_vrays(world);
    define_systems_vrays(world);
zox_end_module(Vrays)

#endif