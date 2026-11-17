entity prefab_menu_play;
entity prefab_menu_play_touch;
entity prefab_tooltip;
entity zoxel_main_menu;
entity prefab_crosshair;
entity prefab_menu_terminal;

#include "tooltip.c"
#include "crosshair.c"
#include "play_touch.c"
#include "play.c"
#include "terminal.c"

void spawn_prefabs_game_ui(ecs *world) {
    prefab_menu_play = spawn_prefab_menu_play(
        world,
        prefab_window);
    prefab_menu_play_touch = spawn_prefab_menu_play_touch(
        world,
        prefab_window);
    prefab_tooltip = spawn_prefab_tooltip(
        world,
        prefab_label2t);
    // its 3D twin: the same prefab functions on a 3D text with a frame behind it (a 3D window's tooltip)
    zox_setv(prefab_tooltip, Prefab3, spawn_prefab_tooltip(world,
        spawn_prefab_label(world, spawn_prefab_text(world, prefab_frame3_twin, prefab_zigel3))));
    ui_window3_tooltip_prefab = prefab_tooltip;
    prefab_menu_terminal = spawn_prefab_menu_terminal(
        world,
        prefab_label2t);
    prefab_crosshair = spawn_prefab_crosshair(
        world,
        prefab_element_frame);
}
