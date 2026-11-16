/*
 * +----------------------------------------------------------+
 * | Zox Module: Equipment                                    |
 * |                                                          |
 * |  Weapons - Armor - Slots - Stats - Loadouts              |
 * |                                                          |
 * +----------------------------------------------------------+
 */
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

// Equipment includes Gear and Body parts in one
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_equipment(ecs* world) {
    zox_module(equipment);
    zox_components_equipment(world);
    zox_systems_equipment(world);
    add_hook_spawn_prefabs(spawn_prefabs_equipment);
    add_taskbar_button(world, (TaskbarData) {
        .index = 7,
        .spawn = &spawn_player_menu_equipment,
        .component_id = zox_id(MenuEquipment),
        .texture_name = "taskbar_equipment",
        .tooltip_text = "Equipment"
    });
#ifdef zox_tests
    zox_tsts_equipment(world);
#endif
}
