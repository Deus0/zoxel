entity spawn_terrain(
    ecs *world,
    entity realm,
    lint seed)
{
    if (!zox_valid(realm)) {
        zox_loge("Realm Invalid [%lu]", realm);
        return 0;
    }
    entity tilemap = zox_get_link(world, realm, Tilemap);
    const float scale = 0.5f; // 0.5f | 1
    entity e = spawn_chunk3_parent_textured(world, prefab_terrain, realm, realm, tilemap, terrain_depth, scale);
    zox_name("terrain");
    zox_setv(e, Seed, seed);
    // Initialize Hashmaps
    zox_set_ptr(e, RegionLinks, (RegionLinks) {
        .value = create_int2_hashmap(32)
    });
    zox_set_ptr(e, TunkLinks, (TunkLinks) {
        .value = create_int2_hashmap(32)
    });
    zox_link(world, realm, TerrainLink, e);
    zox_link(world, e, ClockLink, realm);
    if (tilemap) {
        zox_link(world, tilemap, TerrainLink, e);
    }
    local_terrain = e;
    return e;
}
