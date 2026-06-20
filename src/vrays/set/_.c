#define rayhit_none 0
#define rayhit_terrain 1
#define rayhit_block_vox 2
#define rayhit_character 3
float raycaster_range = 2.5f;
// this should be per scale of the quad?
const float quad_depth_buffer = 0.02f; // 0.02f;
byte debug_ray_big_range = 0;
byte is_debug_rayhit_point = 0;
const uint safety_checks_raycasting = 512;
const float raycast_thickness = 16;
const float hit_character_line_up = 0.02f;
const float hit_block_vox_line_up = 0.02f;
float raygizmo_line_length = 0.02f; // 13f;   // 0.2f
color hit_block_vox_color = { 55, 185, 145, 255 };
color hit_character_color = { 155, 45, 45, 255 };
byte is_debug_mid_voxel = 0;
byte is_slow_gizmos = 0;
