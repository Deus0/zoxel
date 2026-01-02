#include "skybox.c"

void spawn_prefabs_weather(ecs *world) {
    prefab_skybox = spawn_prefab_skybox(world);
}

void spawn_weather(ecs *world) {
#ifndef zox_disable_skybox
    spawn_skybox(world, shader_skybox);
    set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
#endif
}