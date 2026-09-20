entity spawn_prefab_block_vox(
    ecs *world,
    entity prefab)
{
    short length = octree_size(block_depth);
    zox_prefab_child(prefab);
    zox_prefab_name("block_vox");
    zox_add(e, BlockVox);
    zox_add(e, VoxMesh);
    zox_setv(e, CloneVox, 0);
    zox_setv(e, CloneVoxLink, 0);
    // zox_setv(e, ChunkLod, { render_depth_uninitialized });
    zox_setv(e, BlockIndex, 0);
    zox_setv(e, RenderDepth, render_depth_uninitialized);
    zox_setv(e, RenderDepthDirty, zox_dirty_none);
    // BlockLink to original voxel meta data
    zox_setv(e, BlockScale, vox_model_scale);
    // Chunk3
    zox_setv(e, NodeDepth, block_depth);
    zox_setv(e, ChunkSize, int3_single(length));
    // for debug
    /*zox_prefab_set(e, Bounds3D, { (float3) { 0.25f, 0.25f, 0.25f } });
    zox_prefab_set(e, Bounds3Dirty, { 0 });
    prefab_add_cube_lines(world, e, color_white, 0);*/
    return e;
}
