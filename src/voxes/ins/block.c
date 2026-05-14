entity spawn_block_vox(ecs *world, SpawnBlockVox data) {
    /*byte mdepth = block_vox_depth_limits.y;
    if (zox_has(data.prefab, MaxRenderDepth)) {
        mdepth = zox_gett_value(data.prefab, MaxRenderDepth);
    }
    byte ddepth = mdepth - block_vox_depth;
    float scale = 1.0f / (data.scale * powers_of_two[ddepth]);*/
    zox_instance(data.prefab);
    zox_name("block_vox");
    zox_set(e, BlockScale, { data.scale }); // * 0.5f });
    zox_set(e, Position3D, { data.positionf });
    // zox_set(e, Scale1D, { scale });
    zox_set(e, RenderDepth, { data.render_depth });
    zox_set(e, RenderDisabled, { data.render_disabled });
    zox_set(e, BlockIndex, { data.block_index });
    zox_set(e, CloneVox, { 1 });
    zox_set(e, CloneVoxLink, { data.vox });
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);
    /*float scale = block_vox_depth <= 5 ? powers_of_two[5 - block_vox_depth] : 1.0f /  (powers_of_two[block_vox_depth - 5]);
    zox_set(e, Scale1D, { scale });*/
    // zox_log("+ Spawned block vox [%s] with scale: %f", data.prefab, scale);
    return e;
}
