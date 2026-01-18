// TODO: Add quadtree data and chunk2 tag
entity spawn_prefab_tunk2(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("tunk2");
    zox_add_tag(e, Tunk);
    zox_add_tag(e, StreamedChunk);
    zox_prefab_set(e, VoxLink, { 0 });
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    zox_prefab_set(e, Chunk3Stack, { 0 });
    // Maps
    zox_prefab_set(e, BiomeMap, { 0 });
    zox_prefab_set(e, HeightMap, { 0 });
    zox_prefab_set(e, VegetationMap, { 0 });
    zox_prefab_set(e, TownMap, { 0 });
    // Misc
    zox_prefab_set(e, BiomeLink, { 0 });
    return e;
}
