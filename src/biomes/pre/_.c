#include "biome.c"
entity prefab_biome;

void spawn_prefabs_biomes(ecs *world) {
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, BiomeLinks);
    }
    if (prefab_tunk2) {
        zox_setv(prefab_tunk2, BiomeMap, 0);
        zox_setv(prefab_tunk2, BiomeLink, 0);
    }
    if (prefab_camera_game) {
        zox_set(prefab_camera_game, BiomeLink, { 0 });
    }
    prefab_biome = spawn_prefab_biome(world);
}
