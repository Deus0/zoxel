const VoxelLinks* get_first_terrain_voxels(ecs *world, const TerrainLink* TerrainLink_, int count) {
    entity realm = 0;
    for (int i = 0; i < count; i++) {
        zox_sys_i(TerrainLink, link)
        if (!zox_valid(link->value) || !zox_has(link->value, RealmLink)) {
            continue;
        }
        realm = zox_get_value(link->value, RealmLink)
        break;
    }
    if (!zox_valid(realm)) {
        return NULL;
    }
    zox_geter(realm, VoxelLinks, voxels)
    return voxels;
}

// contains air!
void get_block_collisions(ecs *world, const VoxelLinks *voxels, byte *collisions) {
    collisions[0] = 0;    // air
    for (int i = 0; i < voxels->length; i++) {
        const entity block = voxels->value[i];
        if (zox_valid(block)) {
            collisions[i + 1] = zox_gett_value(block, BlockCollider) != zox_block_air;
        } else {
            collisions[i + 1] = 0;
        }
    }
}
