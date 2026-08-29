entity spawn_prefab_vox_generated(ecs *world, entity prefab) {
    short length = octree_size(block_vox_depth);
    zox_prefab_child(prefab);
    zox_add(e, VoxMesh);
    zox_set(e, NodeDepth, { block_vox_depth });
    zox_set(e, ChunkSize, { int3_single(length) });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, Seed, { 0 });
    zox_prefab_set(e, GenerateModel, { zox_generate_model_run });
    zox_prefab_set(e, VoxType, { 0 });
    zox_prefab_set(e, Busy, { 1 });
    return e;
}
