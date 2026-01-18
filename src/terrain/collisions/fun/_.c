const BlockLinks* get_first_terrain_voxels(ecs *world, const TerrainLink* TerrainLink_, int count) {
    entity realm = 0;
    for (int i = 0; i < count; i++) {
        zox_sys_i(TerrainLink, link)
        if (!zox_valid(link->value) || !zox_has(link->value, RealmLink)) {
            continue;
        }
        realm = zox_get_value(link->value, RealmLink);
        break;
    }
    if (!zox_valid(realm)) {
        return NULL;
    }
    zox_geter(realm, BlockLinks, voxels);
    return voxels;
}