const BlockLinks* get_first_terrain_voxels(iter* it) {
    zox_sys_world();
    entity realm = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        realm = zox_get_link(world, terrain, RealmLink);
        break;
    }
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return NULL;
    }
    zox_geter(realm, BlockLinks, voxels);
    return voxels;
}
