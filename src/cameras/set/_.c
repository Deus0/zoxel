byte zox_game_camera_mode = 0;
byte zox_use_post_processing = 1;
byte viewport_downscale = 1;
float viewport_scale = 1;
float3 main_menu_camera_position = (float3) { 0, 0, 3 };
float4 main_menu_camera_rotation = (float4) { 0, 0, 0, 1 };
// debugs
// #define zox_draw_frustum
// #define zox_draw_frustum_planes
// #define zox_debug_camera_frustum
// #define zox_test_camera_frustum
// #define zox_test_frustum2 // realtime
// #define zox_test_frustum
float3 debug_plane_rotation = { 0, 180, 0 };
float3 debug_plane_position = { -64, 16, -64 };
float debug_plane_distance = 128;
float4x4 debug_camera_transform;

// todo: I for raycasting, create a second frustum + second matrix
//      WAIT the precision issue also effects frustum
// todo 2: use double4x4 matrix for view matrix??....
// we know from testing smaller far distance it stops the precision errors
// byte camera_mode = 0; // zox_camera_state_first_person; zox_camera_follow_mode_attach;
#define max_cameras 16
int main_cameras_count = 1;
entity main_cameras[max_cameras];
entity ui_cameras[max_cameras];
#ifdef zox_disable_streaming
    byte zox_cameras_disable_streaming = 1;
#else
    byte zox_cameras_disable_streaming = 0;
#endif
byte camera_follow_mode = 0; //
const float camera_limit_x = 1.25f;
#include "lense.c"

static inline int2 scale_viewport(int2 v) {
    return (int2) { ceil(v.x * viewport_scale), ceil(v.y * viewport_scale) };
}
