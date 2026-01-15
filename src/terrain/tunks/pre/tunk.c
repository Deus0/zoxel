// TODO: Add quadtree data and chunk2 tag
entity spawn_prefab_tunk2(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("tunk2");
    zox_add_tag(e, Tunk);
    zox_add_tag(e, StreamedChunk);
    zox_prefab_set(e, VoxLink, { 0 });
    zox_prefab_set(e, HeightMap, { 0 });
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    return e;
}
