#include "canvas.c"
#include "element.c"
#include "element_textured.c"
entity prefab_canvas;
// entity prefab_element_invisible;  // TODO: Remove this, just use layout2
entity prefab_element;      // has a texture and renderer, good for setting
entity prefab_element_frame;   // rename to element_frame

void prefabs_add_ui_to_player(ecs *world, entity e) {
    // Player -> Devices (move this to device module)
    zox_prefab_set(e, DeviceMode, { 0 });
    zox_prefab_set(e, LastDeviceMode, { 0 });
    zox_prefab_set(e, DeviceModeDirty, { 0 });
    // zox_prefab_set(e, DeviceLinks, { 0 });
    // Player -> UI Links
    zox_prefab_set(e, ElementLinks, { 0 });
    zox_prefab_set(e, CanvasLink, { 0 });
    // this is all UI interaction
    zox_prefab_set(e, NavigatorState, { 0 })
    zox_prefab_set(e, NavigatorTimer, { 0 })
}

void spawn_prefabs_elements(ecs *world) {
    prefab_canvas = spawn_prefab_canvas(world, prefab_layout2_root);
    prefab_element = spawn_prefab_element(world, prefab_layout2);
    prefab_element_frame = spawn_prefab_ui_textured(world, prefab_element);
    // linking
    if (prefab_player) {
        prefabs_add_ui_to_player(world, prefab_player);
    }
}
