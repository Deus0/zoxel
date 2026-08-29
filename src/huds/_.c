/*
 * +------------------------------------------------------------------+
 * | Zox Module: HUDS                                                 |
 * |                                                                  |
 * |  Main Menu - Options - Save Games - Taskbars                     |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */

byte zox_huds3D = 0;
const byte huds_max_list_elements = 128;
const byte zox_huds_header_font_size = 22;
const byte zox_huds_element_font_size = 10;
const byte zox_huds_list_padding = 8;
const byte zox_huds_window_alignment = zox_alignment_centre;
// const float2 zox_huds_window_anchor = (float2) { 0.1f, 0.9f }; // float2_top_left; // float2_centre;
// const float2 zox_huds_window_anchor = float2_centre;
const float2 zox_huds_window_anchor = (float2) { 0.5f, 0.7f };
byte zox_disable_screen_fader = 0;
// TODO: spawn unique canvas per viewport, viewports per player
// TODO: use a byte index instead for menu type
// TODO: find a way to add game modules onto game menu - add stats ui on etc

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "eve/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_huds(ecs* world) {
    zox_module(huds);
    zox_module_dispose(dispose_gameui);
    add_hook_spawn_prefabs(spawn_prefabs_game_ui);
    initialize_hook_taskbar();
    define_components_game_ui(world);
    define_systems_game_ui(world);
    add_hook_spawn_prefabs(spawn_prefabs_game_ui);
    add_hook_spawn_prefabs(zox_events_huds);
    add_taskbar_button((hook_taskbar) {
        .index = 0,
        .spawn = &spawn_menu_paused,
        .component_id = MenuPaused,
        .texture_name = "taskbar_paused",
        .tooltip_text = "Pause Menu"
    });
    add_hook_on_boot(on_boot_game_ui);
}
