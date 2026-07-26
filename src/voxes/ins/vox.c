entity spawn_vox_basic(ecs *world, entity prefab, byte rdepth, byte mdepth) {
    byte ddepth = mdepth - rdepth; // + 1;
    float bscale = ((float) powers_of_two[ddepth]) / 64.0f;
    int3 size = int3_single(powers_of_two[rdepth]);
    zox_instance(prefab);
    zox_set(e, BlockScale, { bscale });
    zox_set(e, ChunkSize, { size });
    zox_set(e, NodeDepth, { rdepth });
    zox_set(e, RenderDepth, { rdepth });
    zox_set(e, MaxRenderDepth, { mdepth });
    // NOTE: Do these in system
    // spawn_gpu_mesh(world, e);
    // spawn_gpu_colors(world, e);
    return e;
}
