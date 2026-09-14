// TODO: Add quadtree data and chunk2 tag
entity spawn_prefab_tunk(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("tunk2");
    zox_add(e, Tunk);
    zox_add(e, StreamedChunk);
    zox_setv(e, GenerateTunk, zox_generate_tunk_start);
    zox_setv(e, TunkLod, 0);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, RenderDistance, 0);
    zox_setv(e, Chunk3Stack, 0);
    zox_setv(e, Chunk3Stacked, 0);
    // Maps
    zox_setv(e, HeightMap, 0);
    return e;
}
