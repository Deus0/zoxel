#include "window.c"
#include "header.c"
#include "body.c"
#include "close_button.c"

entity prefab_window;
entity prefab_window_textured;
entity prefab_header;
entity prefab_body;
entity prefab_close_button;

void prefabs_add_windows_raycaster(ecs *world, entity e) {
    zox_prefab_set(e, WindowRaycasted, { 0 });
    zox_prefab_set(e, WindowTarget, { 0 });
}

void spawn_prefabs_windows(ecs* world) {
    prefab_window = spawn_prefab_window(world, prefab_element_invisible);
    prefab_window_textured = spawn_prefab_window_textured(world, prefab_element_textured);
    prefab_header = spawn_prefab_header(world, prefab_button); // element_textured);
    prefab_body = spawn_prefab_body(world, prefab_element_textured);
    prefab_close_button = spawn_prefab_button_close(world, prefab_button);
    // linking
    if (prefab_player) {
        prefabs_add_windows_raycaster(world, prefab_player);
    }
    if (prefab_device) {
        prefabs_add_windows_raycaster(world, prefab_device);
    }
    if (prefab_zevice_pointer) {
        prefabs_add_windows_raycaster(world, prefab_zevice_pointer);
    }
    // Canvas Stack
    // TODO: Remove WindowToTop - Use CanvasDirty and just find a window without a layer (or at 0 layer) and add that to top of stack
    zox_prefab_set(prefab_canvas, WindowToTop, { 0 });
    zox_prefab_set(prefab_canvas, WindowsLayers, { 0 });
    zox_prefab_set(prefab_canvas, WindowsCount, { 0 });
    // panel
    prefab_grid = spawn_prefab_grid(world, prefab_body); // prefab_layout2);
}
