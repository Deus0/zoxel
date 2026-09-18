zox_tag(CharactersSpawned);
zoxc_byte(CharactersEverSpawned);
zoxc_byte(NpcSpawnZone);
zoxc_state(NpcSpawnZoneDirty);
#define chunk_characters_max 8
zoxc_array(ChunkCharacters, entity, chunk_characters_max);

void zox_components_characters3_terrain(ecs* world) {
    zoxd_tag(CharactersSpawned);
    zoxd_byte(CharactersEverSpawned);
    zoxd_byte(NpcSpawnZone);
    zoxd_state(NpcSpawnZoneDirty);
    zoxd(ChunkCharacters);
}
