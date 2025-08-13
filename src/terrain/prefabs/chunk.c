entity spawn_prefab_chunk_terrain(
    ecs *world,
    const entity prefab,
    const int3 size
) {
    zox_prefab_child(prefab);
    zox_add_tag(e, TerrainChunk);
    zox_add_tag(e, ChunkTextured);
    zox_prefab_set(e, VoxelNodeQueue, { 0 });
    add_components_mesh_textured(world, e);
    // Terrain / Generation
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateChunk, { 0 });
    zox_prefab_set(e, ChunkEntities, { 0, NULL });
    zox_prefab_set(e, BlocksSpawned, { 0 });
    zox_prefab_set(e, ChunkNeighbors, { { 0, 0, 0, 0, 0, 0 } });
    zox_add_tag(e, ChunkDebugger);
    zox_prefab_set(e, RenderDepthDirty, { 0 });
    zox_set(e, RenderDisabled, { 1 });
    // Debug
    zox_prefab_set(e, Bounds3D, { float3_single(1) });  // todo: build this in spawn function
    prefab_add_cube_lines_shrink(world, e, color_white, 0, 0.95f);
    return e;
}
