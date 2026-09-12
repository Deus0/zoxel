zox_sys2(TerrainCharactersDespawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    zox_sys_out(ChunkCharacters);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        zox_sys_o(ChunkCharacters, characters);
        if (dirty->value != zox_dirty_active || !active->value) {
            continue;
        }
        for (int j = 0; j < chunk_characters_max; j++) {
            entity e2 = characters->value[j];
            if (zox_valid(e2)) {
                zox_delete(e2);
            }
        }
        if (dbg_log) {
            zox_log("[%s] Destroying Terrain Characters", zox_getn(e));
        }
        zox_remove(e, CharactersSpawned);
    }
} zox_sys_end(TerrainCharactersDespawnSystem);
