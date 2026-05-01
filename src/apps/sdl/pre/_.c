entity prefab_app_sdl;
byte zox_log_sdl_window = 0;
#include "app.c"

void spawn_prefabs_sdl(ecs* world) {
    prefab_app_sdl = spawn_prefab_app_sdl(world);
}