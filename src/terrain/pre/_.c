entity local_terrain; // todo: remove and just use Realm's Terrain link
#include "terrain.c"
#include "chunk.c"
entity prefab_terrain;
entity prefab_chunk_terrain;

void spawn_prefabs_terrain(ecs *world) {
    prefab_chunk_terrain = spawn_prefab_chunk_terrain(world, prefab_chunk_textured);
    prefab_terrain = spawn_prefab_terrain(world);
    if (prefab_realm) {
        zox_prefab_set(prefab_realm, BlockLinks, { 0 });
        zox_prefab_set(prefab_realm, BlocksDirty, { 0 });
    }
}
