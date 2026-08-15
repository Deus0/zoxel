entity spawn_prefab_chunk_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_add(e, ChunkTextured);
    zox_setv(e, Bounds3D, float3_single(1));
    zox_setv(e, VoxelNodeQueue, 0);
    zox_setv(e, BlockDamageQueue, 0);
    // Links
    zox_setv(e, BlockManagerLink, 0);
    // Events
    zox_setv(e, Initialize, 1);
    zox_setv(e, VoxelNodeEdited, 0);
    return e;
}
