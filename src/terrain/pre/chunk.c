entity spawn_prefab_chunk_terrain(ecs *world, entity p) {
    zox_prefab_child(p);
    // Terrain / Generation
    zox_add_tag(e, TerrainChunk);
    zox_add_tag(e, StreamedChunk);
    zox_prefab_set(e, BlockManagerLink, { 0 });

    zox_prefab_set(e, GenerateChunk, { zox_dirty_none });
    zox_prefab_set(e, VoxelNodeEdited, { zox_dirty_none });
    zox_prefab_set(e, VoxelNodeLoaded, { zox_dirty_none });
    zox_prefab_set(e, VoxelNodeGenerated, { zox_dirty_none });
    zox_prefab_set(e, MeshColorsGenerate, { zox_dirty_none });
    zox_prefab_set(e, MeshColorsDirty, { zox_dirty_none });


    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, VoxelNodeQueue, { 0 });
    // Linked
    zox_prefab_set(e, ChunkNeighbors, { { 0, 0, 0, 0, 0, 0 } });
    zox_prefab_set(e, ChunkEntities, { 0, NULL });
    zox_prefab_set(e, BlocksSpawned, { 0 });
    // Lights
    prefab_add_lights3(world, e, 0);
    // Rendering
    zox_add_tag(e, ChunkTextured);
    zox_set(e, RenderDisabled, { 1 });
    zox_prefab_set(e, RenderDepthDirty, { 0 });
    add_components_mesh_textured(world, e);
    // Debug
    zox_add_tag(e, ChunkDebugger);
    zox_prefab_set(e, Bounds3D, { float3_single(1) });  // todo: build this in spawn function
    prefab_add_cube_lines_shrink(world, e, color_white, 0, 0.95f);
    return e;
}
