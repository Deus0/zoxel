entity spawn_prefab_terrain(ecs *world) {
    zox_prefab();
    zox_prefab_name("terrain");
    zox_add(e, Terrain);
    zox_add(e, TerrainWorld);
    zox_add(e, Loading);
    zox_setv(e, BlockScale, default_vox_scale);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, TilemapLink, 0);
    // Should we add transform late?
    // Links
    zox_prefab_add(e, RegionLinks);
    zox_prefab_add(e, TunkLinks);
    zox_prefab_add(e, ChunkLinks);
    // Stream Event
    zox_setv(e, EventInput, 0);
    zox_setv(e, StreamEndEvent, NULL);
    // zox_setv(e, StreamEndEvent, terrain_state_load_end);
#ifdef zox_is_flatlands
    zox_add(e, FlatTerrain);
#endif
    zox_setv(e, TerrainSpawnQueue, 0);
    return e;
}
