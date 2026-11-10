// Despawns a chunks npcs when its NpcSpawnZone becomes inactive
zox_sys2(TerrainCharactersDespawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        if (dirty->value != zox_dirty_active || active->value) {
            continue;
        }
        zox_each_link(world, e, ChunkCharacters, npc) {
            zox_delete(npc);
        }
        if (dbg_log) {
            zox_log("[%s] Destroying Terrain Characters", zox_getn(e));
        }
        zox_remove(e, CharactersSpawned);
    }
} zox_sys_end(TerrainCharactersDespawnSystem);
