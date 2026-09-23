entity spawn_vox_basic(
    ecs *world,
    entity prefab,
    byte depth,
    byte max_depth)
{
    byte ddepth = max_depth - depth;
    ushort dlength = octree_size(ddepth);
    float block_scale = ((float) dlength) / 64.0f;
    int3 size = int3_single(octree_size(depth));
    zox_instance(prefab);
    zox_setv(e, NodeDepth, depth);
    zox_setv(e, RenderDepth, depth);
    zox_setv(e, MaxRenderDepth, max_depth);
    zox_setv(e, ChunkSize, size);
    zox_setv(e, BlockScale, block_scale);
    initialize_voxel_lock(world, e);
    return e;
}
