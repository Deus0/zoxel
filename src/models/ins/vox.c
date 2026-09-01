entity spawn_vox_basic(
    ecs *world,
    entity prefab,
    byte render_depth,
    byte max_depth)
{
    byte ddepth = max_depth - render_depth;
    ushort dlength = octree_size(ddepth);
    float block_scale = ((float) dlength) / 64.0f;
    int3 size = int3_single(octree_size(render_depth));
    zox_instance(prefab);
    zox_setv(e, BlockScale, block_scale);
    zox_setv(e, ChunkSize, size);
    zox_setv(e, NodeDepth, render_depth);
    zox_setv(e, RenderDepth, render_depth);
    zox_setv(e, MaxRenderDepth, max_depth);
    initialize_voxel_lock(world, e);
    return e;
}
