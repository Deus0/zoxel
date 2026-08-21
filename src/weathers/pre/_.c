#include "shader.c"
#include "skybox.c"
entity shader_skybox;
entity prefab_skybox;

void zox_prefabs_weather(ecs *world) {
    prefab_skybox = spawn_prefab_skybox(world);
}

void zox_shaders_weather(ecs *world) {
    shader_skybox = spawn_shader_skybox(world);
}