// TODO: Add quadtree data and chunk2 tag
entity spawn_prefab_tunk2(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("tunk2");
    zox_add(e, Tunk);
    zox_add(e, StreamedChunk);
    // zox_prefab_set(e, VoxLink, { 0 });
    zox_prefab_set(e, GenerateTunk, { zox_generate_tunk_start });
    zox_prefab_set(e, TunkLod, { 0 });
    zox_set(e, RenderDistance, { 0 });
    // zox_set(e, RenderDistanceDirty, { 0 });
    // zox_prefab_set(e, TunkLodDirty, { zox_dirty_trigger }); // starts dirty
    zox_prefab_set(e, Chunk3Stack, { 0 });
    zox_prefab_set(e, Chunk3Stacked, { 0 });
    // Maps
    zox_prefab_set(e, HeightMap, { 0 });
    // Misc
    zox_prefab_set(e, RegionLink, { 0 });
    return e;
}
