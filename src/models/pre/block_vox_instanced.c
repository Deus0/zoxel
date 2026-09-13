entity spawn_prefab_block_vox_instanced(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("block_vox_instanced");
    // add block stuff onto our vox instanced
    zox_setv(e, BlockIndex, 0);
    zox_setv(e, Color, color_green);
    zox_setv(e, RenderDepthDirty, zox_dirty_trigger);
    // testing
    // prefab_add_cube_lines(world, e, color_white, 0);
    return e;
}
