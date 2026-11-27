#include "shader.c"
#include "cloud.c"
entity shader_cloud;
entity prefab_cloud;
entity prefab_sun;
#include "sun.c"

void zox_prefabs_sky(ecs* world) {
    prefab_cloud = spawn_prefab_cloud(world);
    prefab_sun = spawn_prefab_sun(world);
}

void zox_shaders_sky(ecs* world) {
    shader_cloud = spawn_shader_cloud(world);
}
