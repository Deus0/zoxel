#ifndef zox_stats_ui
#define zox_stats_ui

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(StatsUI) {
    zox_define_components_stats_ui(world);
    define_systems_stats_ui(world);
    add_taskbar_button((hook_taskbar) {
        .index = 2,
        .spawn = &spawn_player_menu_stats,
        .component_id = MenuStats,
        .texture_name = "taskbar_stats",
        .tooltip_text = "Status"
    });
    add_hook_spawn_prefabs(spawn_prefabs_ui_stats);
} zox_end_module(StatsUI);

#endif
