entity spawn_vox_basic(ecs *world, entity p, byte max_node_depth, byte depth) {

    byte ddepth = max_node_depth - depth + 1;
    const float voxel_scale = ((float) ddepth) / 64.0f;
    const int3 chunk_size = int3_single(powers_of_two[depth]);

    zox_instance(p);
    zox_set(e, NodeDepth, { depth });
    zox_set(e, BlockScale, { voxel_scale });
    zox_set(e, ChunkSize, { chunk_size });

    // NOTE: Do instanced models need these??
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);

    return e;
}