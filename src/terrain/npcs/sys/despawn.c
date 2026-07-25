zox_sys2(TerrainCharactersDespawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    zox_sys_out(ChunkCharacters);
    zox_sys_out(CharactersSpawned);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        zox_sys_o(ChunkCharacters, characters);
        zox_sys_o(CharactersSpawned, spawned);
        // spawn again?
        /*if (spawned->value && !characters->length) {
            spawned->value = 0;
        }*/
        if (!(dirty->value == zox_dirty_active && !active->value && spawned->value)) {
            continue;
        }
        if (dbg_log && spawned->value) {
            zox_log("[%s] Destroying Terrain Characters [%i]", zox_getn(e), spawned->value);
        }
        for (int j = 0; j < spawned->value; j++) {
            entity e2 = characters->value[j];
            if (zox_valid(e2) && !zox_has(e2, PlayerCharacter)) {
                zox_delete(e2);
                // remove_at_ChunkEntities(characters, j);
                // ChunkEntities automatically removes it?
            }
        }
        spawned->value = 0;
    }
} zox_sys_end(TerrainCharactersDespawnSystem);
