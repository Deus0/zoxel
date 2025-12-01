entity fps_display_prefab;
entity fps_display;
entity prefab_game_debug_label;
entity prefab_quad_count_label;
entity prefab_tooltip;
entity prefab_frame_taskbar;
entity prefab_taskbar;
entity prefab_device_gizmo;entity prefab_menu_game;
entity prefab_menu_game_touch;
entity zoxel_main_menu;
entity prefab_crosshair;

#include "fps_display.c"
#include "quad_count_label.c"
#include "game_debug_label.c"
#include "tooltip.c"
#include "frame_taskbar.c"
#include "taskbar.c"
#include "device_gizmo.c"
#include "crosshair.c"
#include "menu_game_touch.c"
#include "menu_game.c"

void spawn_prefabs_game_ui(ecs *world) {
    fps_display_prefab = spawn_prefab_fps_display(world);
    spawn_prefab_quad_count_label(world);
    spawn_prefab_game_debug_label(world, prefab_label_background);
    prefab_tooltip = spawn_prefab_tooltip(world, prefab_label_background);
    prefab_frame_taskbar = spawn_prefab_frame_taskbar(world, prefab_frame);
    prefab_taskbar = spawn_prefab_taskbar(world, prefab_element_textured);
    prefab_device_gizmo = spawn_prefab_device_gizmo(world, prefab_icon);
    prefab_menu_game = spawn_prefab_menu_game(world, prefab_layout2);
    prefab_menu_game_touch = spawn_prefab_menu_game_touch(world, prefab_layout2);
    prefab_crosshair = spawn_prefab_crosshair(world, prefab_element_textured);
}
