entity spawn_prefab_terrain(ecs *world) {
    zox_prefab();
    zox_prefab_name("terrain");
    zox_add_tag(e, TerrainWorld);
    zox_prefab_set(e, Loaded, { 0 });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, TilemapLink, { 0 });
    // Links
    zox_set_ptr(e, RegionLinks, (RegionLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, TunkLinks, (TunkLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, ChunkLinks, (ChunkLinks) { .value = create_int3_hashmap(32) });
    // Stream Event
    zox_prefab_set(e, EventInput, { 0 });
    zox_prefab_set(e, StreamEndEvent, { NULL });
#ifdef zox_is_flatlands
    zox_add_tag(e, FlatTerrain);
#endif
    return e;
}
