// NOTE: SPawns a tilemap per realm for our blocks
zox_sys2(TilemapRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TilemapLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TilemapLink, tilemap);
        if (!zox_valid(tilemap->value)) {
            tilemap->value = spawn_tilemap(
                world,
                prefab_tilemap,
                e);
        }
    }
} zox_sys_end(TilemapRealmSpawnSystem);
