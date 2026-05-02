entity spawn_block_vox(ecs *world, SpawnBlockVox data) {

    zox_instance(data.prefab);
    zox_name("block_vox");

    // transforms
    zox_set(e, Position3D, { data.positionf });
    // zox_set(e, Scale1D, { data.scale })
    zox_set(e, BlockScale, { data.scale * 0.5f });

    // rendering
    zox_set(e, RenderDepth, { data.render_depth });
    zox_set(e, RenderDisabled, { data.render_disabled });

    // voxels
    zox_set(e, BlockIndex, { data.block_index });
    zox_set(e, CloneVox, { 1 });
    zox_set(e, CloneVoxLink, { data.vox });

    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);

    // zox_log("+ spawned block vox with scale: %f", data.scale)
    return e;
}
