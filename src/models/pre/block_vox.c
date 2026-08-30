entity spawn_prefab_block_vox(ecs *world, entity prefab) {
    short length = octree_size(block_depth);
    zox_prefab_child(prefab);
    zox_prefab_name("block_vox");
    zox_add(e, BlockVox);
    zox_add(e, VoxMesh);
    zox_prefab_set(e, CloneVox, { 0});
    zox_prefab_set(e, CloneVoxLink, { 0 });
    zox_prefab_set(e, ChunkLod, { render_depth_uninitialized });
    zox_prefab_set(e, BlockIndex, { 0 });
    zox_prefab_set(e, RenderDepth, { render_depth_uninitialized });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    // BlockLink to original voxel meta data
    zox_prefab_set(e, BlockScale, { vox_model_scale });
    // for debug
    zox_prefab_set(e, Bounds3D, { (float3) { 0.25f, 0.25f, 0.25f } });
    zox_prefab_set(e, Bounds3Dirty, { 0 });
    prefab_add_cube_lines(world, e, color_white, 0);
    // Chunk3
    zox_set(e, NodeDepth, { block_depth });
    zox_set(e, ChunkSize, { int3_single(length) });
    // Transforms
    // zox_prefab_set(e, Position3D, { float3_zero });
    // zox_prefab_set(e, Rotation3D, { float4_identity });
    return e;
}
