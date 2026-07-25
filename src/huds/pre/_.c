#include "tooltip.c"
#include "crosshair.c"
#include "play_touch.c"
#include "play.c"
#include "terminal.c"
entity prefab_menu_play;
entity prefab_menu_play_touch;
entity prefab_tooltip;
entity zoxel_main_menu;
entity prefab_crosshair;
entity prefab_menu_terminal;

void spawn_prefabs_game_ui(ecs *world) {
    prefab_menu_play = spawn_prefab_menu_play(world, prefab_window);
    prefab_menu_play_touch = spawn_prefab_menu_play_touch(world, prefab_window);
    prefab_tooltip = spawn_prefab_tooltip(world, prefab_label_textured);
    prefab_menu_terminal = spawn_prefab_menu_terminal(world, prefab_label_textured);
    prefab_crosshair = spawn_prefab_crosshair(world, prefab_element_frame);
}
