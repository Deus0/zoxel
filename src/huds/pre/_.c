// TODO: Move instance functions to ins
entity prefab_quad_count_label;
#include "fps_display.c"
#include "quad_count_label.c"
#include "game_debug_label.c"
#include "tooltip.c"
#include "frame_taskbar.c"
#include "crosshair.c"
#include "play_touch.c"
#include "play.c"
#include "terminal.c"
entity prefab_menu_play;
entity prefab_menu_play_touch;
entity fps_display_prefab;
entity fps_display;
entity prefab_tooltip;
// entity prefab_frame_taskbar;
entity zoxel_main_menu;
entity prefab_crosshair;
entity prefab_menu_terminal;

void spawn_prefabs_game_ui(ecs *world) {
    prefab_menu_play = spawn_prefab_menu_play(world, prefab_window);
    prefab_menu_play_touch = spawn_prefab_menu_play_touch(world, prefab_window);
    prefab_tooltip = spawn_prefab_tooltip(world, prefab_label_textured);
    // prefab_frame_taskbar = spawn_prefab_frame_taskbar(world, prefab_frame);
    prefab_menu_terminal = spawn_prefab_menu_terminal(world, prefab_label_textured);
    prefab_crosshair = spawn_prefab_crosshair(world, prefab_element_textured);
    fps_display_prefab = spawn_prefab_fps_display(world, prefab_label_textured);
    prefab_quad_count_label = spawn_prefab_quad_count_label(world);
}
