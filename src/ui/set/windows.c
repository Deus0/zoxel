// 3D huds (on while XR is): menus and windows as panels in the world ahead of the player (canvas3s), not on the
//  2D canvas; spawn_window builds one so (ui_window3_pose places it), its elements the 3D twins of its 2D prefabs
//  (ui_prefab_for, windows/pre/twins.c)
byte zox_huds3D = 0;
// where a 3D window spawns for a canvas's player (huds/fun/huds3.c huds3_pose), set by huds
void (*ui_window3_pose)(ecs* world, entity canvas, float3* position, float4* rotation) = NULL;
#define ui_window3_metres_per_pixel 0.0045f     // a 3D window's height in metres, of its canvas pixels (ui_scale 1)
// windows
// color window_color = { 48, 42, 36, 255 };
color window_fill = { 255, 22, 22, 99 };
color window_outline = { 44, 44, 44, 177 };
// header
// byte header_font_resolution = 32;
byte header_font_thickness = 1;
byte header_fonto_thickness = 1;
color header_fill = { 122, 22, 22, 122 };
color header_outline = { 44, 44, 44, 222 };
color header_font_fill = { 199, 220, 41, 177 };
color header_font_outline = { 100, 110, 50, 227 };
// footer, darker than its window, its buttons on it
color footer_fill = { 122, 22, 22, 140 };
color footer_outline = { 44, 44, 44, 177 };
// ?
color default_fill_color = { 155, 155, 155, 144 };
color default_outline_color = { 155, 88, 45, 200 };
