#include "shader.c"
#include "cloud.c"
entity shader_cloud;
entity prefab_cloud;

void zox_prefabs_sky(ecs* world) {
    prefab_cloud = spawn_prefab_cloud(world);
}

void zox_shaders_sky(ecs* world) {
    shader_cloud = spawn_shader_cloud(world);
}
