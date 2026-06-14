entity spawn_prefab_chunk_terrain(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    // Terrain / Generation
    zox_add_tag(e, TerrainChunk);
    zox_add_tag(e, StreamedChunk);
    zox_add_tag(e, ChunkTextured);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, Loaded, { 0 });
    zox_prefab_set(e, VoxelNodeEdited, { 0 });
    zox_set(e, RenderDisabled, { 1 });
    // GPU
    zox_prefab_set(e, MeshUVs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    add_gpu_uvs(world, e);
    add_gpu_texture(world, e);
    add_gpu_colors(world, e);
    // Links
    zox_prefab_set(e, TunkLink, { 0 });
    zox_prefab_set(e, BlockManagerLink, { 0 });
    zox_prefab_set(e, ChunkNeighbors, { { 0, 0, 0, 0, 0, 0 } });
    zox_prefab_set(e, ChunkEntities, { 0 });
    zox_prefab_set(e, BlocksSpawned, { 0 });
    // Queues
    zox_prefab_set(e, VoxelNodeQueue, { 0 });
    // Dirty States
    zox_prefab_set(e, Generate, { zox_dirty_none });
    zox_prefab_set(e, MeshColorsGenerate, { zox_dirty_none });
    zox_prefab_set(e, MeshColorsDirty, { zox_dirty_none });
    zox_prefab_set(e, Bounds3Dirty, { zox_dirty_none });
    // Lights
    prefab_add_lights3(world, e, 0);
    // Debug
    zox_add_tag(e, ChunkDebugger);
    zox_prefab_set(e, Bounds3D, { float3_single(1) });
    prefab_add_cube_lines_shrink(world, e, color_white, 0, 0.95f);
    return e;
}
