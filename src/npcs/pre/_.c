

void spawn_prefabs_characters3_terrain(ecs* world) {
    if (prefab_chunk_terrain) {
        zox_setv(prefab_chunk_terrain, ChunkCharacters, { 0 } );
        zox_setv(prefab_chunk_terrain, NpcSpawnZone, 0);
        zox_setv(prefab_chunk_terrain, NpcSpawnZoneDirty, 0);
    }
}
