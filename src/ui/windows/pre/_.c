#include "window.c"

entity prefab_window;
entity prefab_window_textured;

void spawn_prefabs_windows(ecs* world) {
    prefab_window           = spawn_prefab_window(world, prefab_element_invisible);
    prefab_window_textured  = spawn_prefab_window_textured(world, prefab_element_textured);
}
