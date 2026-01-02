entity prefab_app;
entity main_app;

#include "app.c"

void spawn_prefabs_apps(ecs *world) {
    prefab_app = spawn_prefab_app(world, default_window_position, default_window_size);
}