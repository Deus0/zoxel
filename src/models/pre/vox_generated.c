entity spawn_prefab_vox_generated(ecs *world, entity prefab) {
    short length = octree_size(block_depth);
    zox_prefab_child(prefab);
    zox_add(e, VoxMesh);
    zox_setv(e, NodeDepth, block_depth);
    zox_setv(e, ChunkSize, int3_single(length));
    zox_setv(e, Color, color_white);
    zox_setv(e, Seed, 0);
    zox_setv(e, GenerateModel, zox_generate_model_run);
    zox_setv(e, VoxType, 0);
    return e;
}
