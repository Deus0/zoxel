void spawn_realm_tilemaps(ecs *world, const entity realm) {

    // spawn a tilemap!
    if (!zox_has(realm, TilemapLink)) {
        return;
    }
    zox_geter_value(realm, TilemapLink, entity, old);
    // if (old) return; // TODO: Temp; Remove when crashes gone

    if (zox_valid(old)) {
        zox_delete(old);
    }

    entity tilemap = spawn_tilemap(world, prefab_tilemap);
    zox_set(realm, TilemapLink, { tilemap });
    if (tilemap) {
        zox_set(tilemap, RealmLink, { realm });
    }
}