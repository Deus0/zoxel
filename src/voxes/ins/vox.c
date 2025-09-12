entity spawn_vox_basic(
    ecs *world,
    const entity prefab,
    byte max_node_depth,
    byte node_depth
) {
    byte ddepth = max_node_depth - node_depth + 1;
    const float voxel_scale = ((float) ddepth) / 64.0f;
    const int3 chunk_size = int3_single(powers_of_two[node_depth]);

    zox_instance(prefab);
    zox_set(e, NodeDepth, { node_depth });
    zox_set(e, BlockScale, { voxel_scale });
    zox_set(e, ChunkSize, { chunk_size });
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);

    return e;
}