entity spawn_prefab_chunk_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    // Tags
    zox_add_tag(e, ChunkTextured);
    // Links
    zox_prefab_set(e, BlockManagerLink, { 0 });
    // Bounds, used for Frustum Culling
    zox_prefab_set(e, Bounds3D, { float3_single(1) });
    zox_prefab_set(e, Bounds3Dirty, { zox_dirty_none });
    // States
    zox_prefab_set(e, VoxelNodeEdited, { 0 });
    // Events
    zox_prefab_set(e, Busy, { 0 });
    zox_prefab_set(e, Ready, { 0 });
    zox_prefab_set(e, Initialize, { 1 }); // For Loading
    // Neighbors and Updates
    zox_prefab_set(e, VoxelNodeQueue, { 0 });
    zox_prefab_set(e, BlockDamageQueue, { 0 });
    return e;
}
