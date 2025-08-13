ecs_entity_t local_terrain; // todo: remove and just use Realm's TerrainLink
#include "terrain.c"
#include "chunk.c"
ecs_entity_t prefab_terrain;
ecs_entity_t prefab_chunk_terrain;
ecs_entity_t prefab_terrain_chunk_flat;

void spawn_prefabs_terrain(ecs_world_t *world) {
    const byte depth = terrain_depth;
    const byte length = powers_of_two[depth];
    const int3 terrain_chunk_size = (int3) { length, length, length }; // 8 *
    prefab_chunk_terrain = spawn_prefab_chunk_terrain(world, prefab_chunk, terrain_chunk_size);
    prefab_terrain_chunk_flat = spawn_prefab_chunk_terrain(world, prefab_chunk, terrain_chunk_size);
    zox_add_tag(prefab_terrain_chunk_flat, FlatlandChunk)
    prefab_terrain = spawn_prefab_terrain(world);
    if (prefab_realm) {
        zox_prefab_set(prefab_realm, ModelLinks, { 0, NULL })
        zox_prefab_set(prefab_realm, VoxelLinks, { 0, NULL })
        zox_prefab_set(prefab_realm, VoxelsDirty, { 0 })
    }
}
