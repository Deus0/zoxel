#include "window.c"

entity prefab_window_invisible;
entity prefab_window;

void spawn_prefabs_windows(ecs* world) {
    prefab_window_invisible     = spawn_prefab_window_invisible(world, prefab_element_invisible);
    prefab_window               = spawn_prefab_window(world, prefab_element_textured);
}
