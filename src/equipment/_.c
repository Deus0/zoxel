/*
 * +------------------------------------------------------------------+
 * | Zox Module: Equipment                                            |
 * |                                                                  |
 * |  Weapons - Armor - Slots - Stats - Loadouts                      |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

// Equipment includes Gear and Body parts in one
void import_equipment(ecs* world) {
    zox_module(equipment);
    define_components_equipment(world);
    define_systems_equipment(world);
    add_hook_spawn_prefabs(spawn_prefabs_equipment);
    add_taskbar_button((hook_taskbar) {
        .index = 7,
        .spawn = &spawn_player_menu_equipment,
        .component_id = MenuEquipment,
        .texture_name = "taskbar_equipment",
        .tooltip_text = "Equipment"
    });
}