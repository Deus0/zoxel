entity spawn_prefab_terrain(ecs *world) {
    zox_prefab();
    zox_prefab_name("terrain");
    zox_add_tag(e, TerrainWorld);
    zox_prefab_set(e, Loaded, { 0 });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, TilemapLink, { 0 });
    // Should we add transform late?
    // Links
    zox_prefab_add(e, RegionLinks);
    zox_prefab_add(e, TunkLinks);
    zox_prefab_add(e, ChunkLinks);
    // Stream Event
    zox_prefab_set(e, EventInput, { 0 });
    zox_prefab_set(e, StreamEndEvent, { NULL });
#ifdef zox_is_flatlands
    zox_add_tag(e, FlatTerrain);
#endif
    return e;
}
