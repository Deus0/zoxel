
byte level_label_show_experience = 0;

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"

void import_statsui(ecs* world) {
    zox_module(statsui);
    zox_components_stats_ui(world);
    zox_systems_stats_ui(world);
    add_hook_spawn_prefabs(spawn_prefabs_ui_stats);
    add_hook_spawn_prefabs(zox_events_stats_ui);
    add_taskbar_button(world, (TaskbarData) {
        .index = 2,
        .spawn = &spawn_player_menu_stats,
        .component_id = zox_id(MenuStats),
        .texture_name = "taskbar_stats",
        .tooltip_text = "Status"
    });
}
