

void spawn_prefabs_characters3_terrain(ecs* world) {
    if (prefab_chunk_terrain) {
        zox_prefab_set(prefab_chunk_terrain, NpcSpawnZone, { 1 });
        zox_prefab_set(prefab_chunk_terrain, CharactersSpawned, { 0 });
        zox_prefab_set(prefab_chunk_terrain, CharactersEverSpawned, { 0 });
        zox_prefab_set(prefab_chunk_terrain, NpcSpawnZoneDirty, { 0 });
    }
}
