entity spawn_vox_basic(ecs *world, entity p, byte mdepth, byte depth) {
    byte ddepth = mdepth - depth; // + 1;
    // const float bscale = ((float) ddepth) / 64.0f;
    float bscale = ((float) powers_of_two[ddepth]) / 64.0f;
    const int3 chunk_size = int3_single(powers_of_two[depth]);
    zox_instance(p);
    zox_set(e, NodeDepth, { depth });
    zox_set(e, BlockScale, { bscale });
    zox_set(e, ChunkSize, { chunk_size });
    // NOTE: Do instanced models need these??
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);
    return e;
}
