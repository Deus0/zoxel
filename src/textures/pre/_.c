#include "texture.c"
#include "noise_texture.c"
#include "dirt_texture.c"
#include "tilemap.c"
entity prefab_texture;
entity prefab_texture_noise;
entity prefab_tilemap;

void spawn_prefabs_textures_core(ecs *world) {
    if (prefab_realm) {
        // zox_prefab_add(prefab_realm, TilemapLink) // realms link
        zox_prefab_set(prefab_realm, TilemapLink, { 0 }) // realms link
    }
    prefab_texture = spawn_prefab_texture(world, voxel_texture_size);
    prefab_texture_noise = spawn_prefab_texture_noise(world, voxel_texture_size);
    prefab_tilemap = spawn_prefab_tilemap(world);
}