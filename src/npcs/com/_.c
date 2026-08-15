zoxc_byte(CharactersSpawned);
zoxc_byte(CharactersEverSpawned);
zoxc_byte(NpcSpawnZone);
zoxc_state(NpcSpawnZoneDirty);
#define chunk_characters_max 8
zoxc_array(ChunkCharacters, entity, chunk_characters_max);

void define_components_characters3_terrain(ecs* world) {
    zoxd_byte(CharactersSpawned);
    zoxd_byte(CharactersEverSpawned);
    zoxd_byte(NpcSpawnZone);
    zoxd_state(NpcSpawnZoneDirty);
    zoxd(ChunkCharacters);
}
