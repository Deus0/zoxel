#include "biome.c"
entity prefab_biome;

void spawn_prefabs_biomes(ecs *world) {
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, BiomeLinks);
    }
    prefab_biome = spawn_prefab_biome(world);
}