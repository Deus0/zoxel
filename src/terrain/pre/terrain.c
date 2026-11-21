entity spawn_prefab_terrain(ecs *world) {
    zox_prefab();
    zox_prefab_name("terrain");
    zox_add(e, Terrain);
    zox_add(e, TerrainWorld);
    zox_add(e, Loading);
    zox_setv(e, BlockScale, default_vox_scale);
    zox_setv(e, NodeDepth, 0);
    // its clock, its realm's (ClockLink)
    zox_setv(e, ClockTime, 0);
    zox_setv(e, ClockDelta, 0);
    // Should we add transform late?
    // Links
    zox_add(e, RegionLinks);
    zox_add(e, TunkLinks);
    zox_add(e, ChunkLinks);
    zox_setv(e, WaterFlows, 0);
    // Stream Event
    zox_setv(e, EventInput, 0);
    zox_setv(e, StreamEndEvent, NULL);
    // zox_setv(e, StreamEndEvent, terrain_state_load_end);
#ifdef zox_is_flatlands
    zox_add(e, FlatTerrain);
#endif
    zox_setv(e, TerrainSpawnQueue, 0);
    // gpu meshing's (lights3/sys/gpu_mesh.c), made on its first gpu mesh: here so that's a set, not an add (an add at
    //  its first mesh was lost, its program and grids made twice, one leaked)
    zox_setv(e, ComputeGPULink, 0);
    zox_setv(e, GridGPULink, guint2_zero);
    return e;
}
