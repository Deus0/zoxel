#define rayhit_none 0
#define rayhit_terrain 1
#define rayhit_block_vox 2
#define rayhit_character 3

byte debug_ray_big_range = 0;
const uint safety_checks_raycasting = 512;
const float raycast_thickness = 16;
const float raycaster_quad_thickness = 4;
color hit_terrain_color = { 2, 2, 2, 155 };
color hit_character_color = { 155, 45, 45, 65 };
color hit_block_vox_color = { 55, 185, 145, 45 };
