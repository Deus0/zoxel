#include "window.c"
#include "header.c"
#include "body.c"
#include "close_button.c"

entity prefab_window;
entity prefab_window_textured;
entity prefab_header;
entity prefab_body;
entity prefab_close_button;

void spawn_prefabs_windows(ecs* world) {
    prefab_window           = spawn_prefab_window(world, prefab_element_invisible);
    prefab_window_textured  = spawn_prefab_window_textured(world, prefab_element_textured);
    prefab_header           = spawn_prefab_header(world, prefab_element_textured);
    prefab_body             = spawn_prefab_body(world, prefab_element_textured);
    prefab_close_button     = spawn_prefab_button_close(world, prefab_button);
}
