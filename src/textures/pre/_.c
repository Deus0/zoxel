#include "texture.c"
#include "noise_texture.c"
#include "dirt_texture.c"
#include "tilemap.c"
entity prefab_texture;
entity prefab_texture_noise;
entity prefab_tilemap;

extern byte block_depth;

void spawn_prefabs_textures_core(ecs *world) {
    if (prefab_realm) {
        // zox_prefab_add(prefab_realm, TilemapLink) // realms link
        zox_prefab_set(prefab_realm, TilemapLink, { 0 }) // realms link
    }

    int2 size = int2_single(powers_of_two[block_depth]);
    prefab_texture = spawn_prefab_texture(world, size);
    prefab_texture_noise = spawn_prefab_texture_noise(world, size);

    prefab_tilemap = spawn_prefab_tilemap(world);
}
