#include "canvas.c"
#include "element.c"
#include "element_textured.c"
entity prefab_canvas;
// entity prefab_element_invisible;  // TODO: Remove this, just use layout2
entity prefab_element;      // has a texture and renderer, good for setting
entity prefab_element_frame;   // rename to element_frame


// Player -> Devices (move this to device module)
void prefabs_add_to_player_inputs(ecs *world, entity e) {
    zox_prefab_set(e, DeviceMode, { 0 });
    zox_prefab_set(e, LastDeviceMode, { 0 });
    zox_prefab_set(e, DeviceModeDirty, { 0 });
}

void prefabs_add_to_player_uis(ecs *world, entity e) {
    // Player -> UI Links
    zox_prefab_set(e, ElementLinks, { 0 });
}

// this is all UI interaction
void prefab_add_navigator(ecs *world, entity e) {
    zox_prefab_add(e, DeviceNavigator);
    zox_prefab_set(e, NavigatorState, { 0 })
    zox_prefab_set(e, NavigatorTimer, { 0 })
}

void spawn_prefabs_elements(ecs *world) {
    prefab_canvas = spawn_prefab_canvas(world, prefab_layout2_root);
    prefab_element = spawn_prefab_element(world, prefab_layout2);
    prefab_element_frame = spawn_prefab_ui_textured(world, prefab_element);
    prefab_add_navigator(world, prefab_gamepad);
    // linking
    if (prefab_player) {
        prefabs_add_to_player_inputs(world, prefab_player);
        prefabs_add_to_player_uis(world, prefab_player);
    }
}
