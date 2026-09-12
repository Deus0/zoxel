entity spawn_prefab_chunk_terrain(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("chunk_terrain");
    zox_setv(e, Seed, 0);
    zox_add(e, TerrainChunk);
    zox_add(e, Saver);
    zox_add(e, StreamedChunk);
    zox_add(e, DisableTransform);
    zox_set(e, ChunkEntities, { 0 });
    prefab_add_lights3(world, e, 0);
    // Events
    zox_setv(e, RenderDistanceDirty, zox_dirty_trigger);
    zox_setv(e, ChunkLodDirty, zox_chunk_lod_dirty_start);
    zox_add(e, FindNeighbors);
    // Debug
/*#ifdef zox_debug
    zox_add(e, ChunkDebugger);
    prefab_add_cube_lines_shrink(world, e, color_white, 0, 0.95f);
#endif*/
    return e;
}
