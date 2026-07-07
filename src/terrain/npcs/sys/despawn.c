zox_sys2(Characters3DespawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    zox_sys_out(CharactersSpawned);
    zox_sys_out(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        zox_sys_o(CharactersSpawned, spawned);
        zox_sys_o(ChunkEntities, entities);
        if (!(dirty->value == zox_dirty_active && !active->value && spawned->value)) {
            continue;
        }
        if (dbg_log) {
            zox_logv("Destroying Terrain Characters [%i]", entities->length);
        }
        for (int j = entities->length - 1; j >= 0; j--) {
            entity e2 = entities->value[j];
            if (!zox_has(e2, PlayerCharacter)) {
                zox_delete(e2);
                remove_at_ChunkEntities(entities, j);
            }
        }
        spawned->value = 0;
    }
} zox_sys_end(Characters3DespawnSystem);
