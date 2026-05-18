#include "canvas.c"
#include "shell.c"
#include "element.c"
#include "element_textured.c"
#include "canvas_overlay.c"
entity prefab_canvas;
entity prefab_element_invisible;  // TODO: Remove this, just use layout2
entity prefab_element_shell;      // has a texture and renderer, good for setting
entity prefab_element_ready;      // ready for generating textures
entity prefab_element_textured;   // rename to textured_frame
entity prefab_canvas_overlay;

void prefabs_add_ui_to_player(ecs *world, const entity e) {
    // Player -> Devices (move this to device module)
    zox_prefab_set(e, DeviceMode, { 0 })
    zox_prefab_set(e, DeviceModeDirty, { 0 })
    zox_prefab_set(e, DeviceLinks, { 0 });
    // Player -> UI Links
    zox_prefab_set(e, ElementLinks, { 0 });
    zox_prefab_set(e, CanvasLink, { 0 });
    // this is all UI interaction
    zox_prefab_set(e, NavigatorState, { 0 })
    zox_prefab_set(e, NavigatorTimer, { 0 })
}

void spawn_prefabs_elements(ecs *world) {
    prefab_canvas = spawn_prefab_canvas(world);
    prefab_element_invisible = spawn_prefab_element_invisible(world, prefab_layout2);
    // has a texture also
    prefab_element_shell = spawn_prefab_element_shell(world, prefab_element_invisible);
    // has a texture also
    prefab_element_ready = spawn_prefab_element_ready(world, prefab_element_shell);
    // has a frame texture
    prefab_element_textured = spawn_prefab_ui_textured(world, prefab_element_ready);
    // more stuffs
    prefab_canvas_overlay = spawn_prefab_canvas_overlay(world, prefab_element_textured);
    // linking
    if (prefab_player) {
        prefabs_add_ui_to_player(world, prefab_player);
    }
}
