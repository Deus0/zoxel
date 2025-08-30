#ifndef zox_stats_ui
#define zox_stats_ui

zox_tag(Statbar);
zox_tag(MenuStats);
zox_tag(StatsLabel);
zox_tag(Healthbar);

#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(StatsUI)
    zoxd_tag(Statbar);
    zoxd_tag(MenuStats);
    zoxd_tag(StatsLabel);
    zoxd_tag(Healthbar);
    define_systems_stats_ui(world);
    add_taskbar_button((hook_taskbar) {
        .spawn = &spawn_player_menu_stats,
        .component_id = MenuStats,
        .texture_name = "taskbar_stats"
    });
    add_hook_spawn_prefabs(spawn_prefabs_ui_stats);
zox_end_module(StatsUI)

#endif