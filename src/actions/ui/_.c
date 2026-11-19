#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

// void zox_actions_ui_taskbar(ecs* world) { }

void import_actions_ui(ecs* world) {
    zox_module(actions_ui);
    zox_components_actions_ui(world);
    zox_systems_actions_ui(world);
    zox_on_add(
        on_action_swapped,
        [none] actions_ui.ActionSwapped,
    );
    // add_hook_spawn_prefabs(zox_actions_ui_taskbar);
    add_taskbar_button(world, (TaskbarData) {
        .index = 1,
        .spawn = &spawn_menu_actions,
        .component_id = zox_id(MenuActions),
        .texture_name = "taskbar_actions",
        .tooltip_text = "Actionbar"
    });
}
