entity spawn_prefab_block_vox(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("block_vox");
    zox_add_tag(e, BlockVox);
    zox_add_tag(e, VoxMesh);
    zox_prefab_set(e, CloneVox, { 0});
    zox_prefab_set(e, CloneVoxLink, { 0 });
    zox_prefab_set(e, ChunkLod, { render_depth_spawning });
    zox_prefab_set(e, BlockIndex, { 0 });
    zox_prefab_set(e, RenderDepth, { render_depth_spawning });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    // BlockLink to original voxel meta data
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, BlockScale, { vox_model_scale });
    // for debug
    zox_prefab_set(e, Bounds3D, { (float3) { 0.25f, 0.25f, 0.25f } });
    prefab_add_cube_lines(world, e, color_white, 0);
    return e;
}
