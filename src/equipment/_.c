#ifndef zoxm_equipment
#define zoxm_equipment

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

// Equipment includes Gear and Body parts in one
zox_begin_module(Equipment)
    add_hook_spawn_prefabs(spawn_prefabs_equipment);
    add_hook_spawned_character3D(&spawn_character_equipment);
    define_components_equipment(world);
    define_systems_equipment(world);
    add_taskbar_button((hook_taskbar) {
        .index = 6,
        .spawn = &spawn_player_menu_equipment,
        .component_id = MenuEquipment,
        .texture_name = "taskbar_equipment"
    });
zox_end_module(Equipment)

#endif