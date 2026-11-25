// a chunk3 parent that streams its chunks round its streamers, generating them (chunks3/pre/chunk3_parent.c)
entity spawn_prefab_terrain(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("terrain");
    zox_add(e, Terrain);
    zox_add(e, TerrainWorld);
    zox_add(e, Loading);
    // its clock, its realm's (ClockLink)
    zox_setv(e, ClockTime, 0);
    zox_setv(e, ClockDelta, 0);
    // Should we add transform late?
    // Links
    zox_add(e, RegionLinks);
    zox_add(e, TunkLinks);
    zox_setv(e, WaterFlows, 0);
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
