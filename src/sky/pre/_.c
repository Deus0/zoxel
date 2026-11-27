#include "shader.c"
#include "cloud.c"
#include "sun.c"
entity shader_cloud;
entity prefab_cloud;
entity prefab_sun;

void zox_prefabs_sky(ecs* world) {
    prefab_cloud = spawn_prefab_cloud(world);
    prefab_sun = spawn_prefab_sun(world, prefab_realm);
}

void zox_shaders_sky(ecs* world) {
    shader_cloud = spawn_shader_cloud(world);
}
