entity local_terrain; // todo: remove and just use Realm's TerrainLink
#include "terrain.c"
#include "chunk.c"
entity prefab_terrain;
entity prefab_chunk_terrain;
entity prefab_terrain_chunk_flat;

void spawn_prefabs_terrain(ecs *world) {
    prefab_chunk_terrain = spawn_prefab_chunk_terrain(world, prefab_chunk);
    prefab_terrain_chunk_flat = spawn_prefab_chunk_terrain(world, prefab_chunk);
    zox_add_tag(prefab_terrain_chunk_flat, FlatlandChunk);
    prefab_terrain = spawn_prefab_terrain(world);
    if (prefab_realm) {
        zox_prefab_set(prefab_realm, ModelLinks, { 0 });
        zox_prefab_set(prefab_realm, BlockLinks, { 0 });
        zox_prefab_set(prefab_realm, BlocksDirty, { 0 });
    }
    if (zox_valid(prefab_character3)) {
        zox_prefab_character_set(TerrainLink, { 0 });
    }
}
