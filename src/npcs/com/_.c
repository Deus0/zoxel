zox_tag(CharactersSpawned);
zoxc_byte(CharactersEverSpawned);
zoxc_byte(NpcSpawnZone);
zoxc_state(NpcSpawnZoneDirty);
zox_tag(ChunkCharacters);  // links a terrain chunk to the npcs it spawned, their home
zoxc_arrayd(PopulationMap, byte);  // a region's npc density a chunk column, 0 to 255 (sys/population.c)
zoxc_byte(NpcHome);         // an npc's index in its home chunk's spawns, it comes back as itself

// Npcs are deleted with the chunk that spawned them (their home), when it unloads
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
    zoxd_arrayd(PopulationMap);
    zoxd_byte(NpcHome);
    zox_on_unlink(on_unlink_ChunkCharacters, ChunkCharacters);
}
