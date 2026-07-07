entity spawn_prefab_chunk_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_add_tag(e, ChunkTextured);
    // Links
    zox_prefab_set(e, BlockManagerLink, { 0 });
    zox_prefab_set(e, ChunkNeighbors, { 0 }); // 0, 0, 0, 0, 0, 0
    // GPU
    zox_set(e, RenderDisabled, { 1 });
    zox_prefab_set(e, MeshUVs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    add_gpu_uvs(world, e);
    add_gpu_texture(world, e);
    add_gpu_colors(world, e);
    // Byte Properties
    zox_prefab_set(e, MeshReady, { 0 });
    zox_prefab_set(e, VoxelNodeEdited, { 0 });
    zox_prefab_set(e, Loaded, { 0 });
    zox_prefab_set(e, Busy, { 1 });         // NOTE: Busy Updating stuff
    zox_prefab_set(e, Ready, { 0 });        // NOTE: Ready to upload to gpu
    // Bounds, used for Frustum Culling
    zox_prefab_set(e, Bounds3D, { float3_single(1) });
    zox_prefab_set(e, Bounds3Dirty, { zox_dirty_none });
    // Neighbors and Updates
    zox_prefab_set(e, VoxelNodeQueue, { 0 });
    zox_prefab_set(e, BlockDamageQueue, { 0 });
    return e;
}
