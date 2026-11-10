zox_tag(CharactersSpawned);
zoxc_byte(CharactersEverSpawned);
zoxc_byte(NpcSpawnZone);
zoxc_state(NpcSpawnZoneDirty);
zox_tag(ChunkCharacters);  // links a terrain chunk to the npcs it spawned

// Npcs are deleted with the chunk that spawned them
void on_unlink_ChunkCharacters(ecs* world, entity chunk, entity npc) {
    if (!zox_valid(chunk) && zox_valid(npc)) {
        zox_delete(npc);
    }
}

void zox_components_characters3_terrain(ecs* world) {
    zoxd_tag(CharactersSpawned);
    zoxd_byte(CharactersEverSpawned);
    zoxd_byte(NpcSpawnZone);
    zoxd_state(NpcSpawnZoneDirty);
    zoxd_nf_links(ChunkCharacters);
    zox_on_unlink(on_unlink_ChunkCharacters, ChunkCharacters);
}
