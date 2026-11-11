entity spawn_prefab_block_vox_instanced(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("block_vox_instanced");
    // add block stuff onto our vox instanced
    zox_setv(e, BlockIndex, 0);
    // its voxel in the chunk, and how lit it is there (vodes3 updates it when the chunk's lights change)
    zox_setv(e, VoxelLocalPosition, byte3_zero);
    zox_setv(e, Brightness, 1);
    zox_setv(e, Color, color_green);
    zox_setv(e, RenderDepthDirty, zox_dirty_trigger);
    // testing
    // prefab_add_cube_lines(world, e, color_white, 0);
    return e;
}
