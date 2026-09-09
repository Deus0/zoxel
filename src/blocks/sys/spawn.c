// NOTE: SPawns a tilemap per realm for our blocks
zox_sys2(TilemapRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity tilemap = zox_get_link(world, e, Tilemap);
        if (!zox_valid(tilemap)) {
            tilemap = spawn_tilemap(
                world,
                prefab_tilemap,
                e);
            zox_link(world, e, Tilemap, tilemap);
        }
    }
} zox_sys_end(TilemapRealmSpawnSystem);
