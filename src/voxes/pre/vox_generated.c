entity spawn_prefab_vox_generated(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    zox_prefab_set(e, Color, { color_white });
    zox_add_tag(e, VoxMesh);
    zox_set(e, NodeDepth, { block_vox_depth });
    int3 size3 = int3_single(powers_of_two[block_vox_depth]);
    zox_set(e, ChunkSize, { size3 });
    return e;
}
