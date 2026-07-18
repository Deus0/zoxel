entity spawn_prefab_chunk_terrain(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    // Terrain / Generation
    zox_add_tag(e, TerrainChunk);
    zox_add_tag(e, StreamedChunk);
    zox_add_tag(e, DisableTransform);
    // Generation
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateChunk, { 0 });
    zox_prefab_set(e, Loaded, { 0 });
    zox_prefab_set(e, ChunkLodDirty, { 1 });
    // Tunks
    zox_prefab_set(e, TunkLink, { 0 });
    // Vodes
    zox_prefab_set(e, ChunkEntities, { 0 });
    zox_prefab_set(e, BlocksSpawned, { 0 });
    // Lights
    prefab_add_lights3(world, e, 0);
    // Debug
    zox_add_tag(e, ChunkDebugger);
    prefab_add_cube_lines_shrink(world, e, color_white, 0, 0.95f);
    return e;
}
