// TODO: Move instance functions to ins
entity prefab_quad_count_label;
entity prefab_game_debug_label;
entity prefab_device_gizmo;

#include "game.c"
#include "fps_display.c"
#include "quad_count_label.c"
#include "game_debug_label.c"
#include "tooltip.c"
#include "frame_taskbar.c"
#include "taskbar.c"
#include "device_gizmo.c"
#include "crosshair.c"
#include "play_touch.c"
#include "play.c"
#include "terminal.c"
#include "paused.c"

entity prefab_menu_game;

entity prefab_menu_play;
entity prefab_menu_play_touch;
// entity prefab_menu_paused;

entity fps_display_prefab;
entity fps_display;
entity prefab_tooltip;
entity prefab_frame_taskbar;
entity prefab_taskbar;
entity zoxel_main_menu;
entity prefab_crosshair;
entity prefab_menu_terminal;

void spawn_prefabs_game_ui(ecs *world) {
    // prefab_layout2 prefab_window_invisible
    prefab_menu_game = spawn_prefab_menu_game(world, prefab_window_invisible);

    prefab_menu_play = spawn_prefab_menu_play(world, prefab_menu_game);
    prefab_menu_play_touch = spawn_prefab_menu_play_touch(world, prefab_menu_game);
    // prefab_menu_paused = spawn_prefab_menu_paused(world, prefab_menu_game);

    spawn_prefab_game_debug_label(world, prefab_label_background);
    prefab_tooltip = spawn_prefab_tooltip(world, prefab_label_background);
    prefab_frame_taskbar = spawn_prefab_frame_taskbar(world, prefab_frame);
    prefab_taskbar = spawn_prefab_taskbar(world, prefab_element_textured);

    prefab_menu_terminal = spawn_prefab_menu_terminal(world, prefab_label_background);

    prefab_crosshair = spawn_prefab_crosshair(world, prefab_element_textured);
    prefab_device_gizmo = spawn_prefab_device_gizmo(world, prefab_icon);
    fps_display_prefab = spawn_prefab_fps_display(world, prefab_label_background);
    prefab_quad_count_label = spawn_prefab_quad_count_label(world);
}
