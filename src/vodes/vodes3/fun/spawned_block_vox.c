void spawned_block_vox(
    ecs *world,
    spawned_block_data* data
) {
    if (!zox_has(data->block, BlockPrefabLink)) {
        return;
    }
    if (!data->node) {
        zox_log_error("null node in [spawned_block_vox]")
        return;
    }
    // gett block data
    zox_geter_value(data->block, BlockPrefabLink, entity, prefab);
    zox_geter_value_safe(data->block, ModelLink, entity, vox);
    SpawnBlockVox spawn_data = {
        .prefab = prefab,
        .vox = vox,
        .block_index = data->block_index,
        .positionl = data->positionl,
        .positionv = data->positionv,
        .positionf = data->positionf,
        .scale = data->scale,
        .render_depth = data->render_depth,
        .render_disabled = data->render_disabled,
    };

    entity e2;
    if (zox_has(prefab, BlockVox)) {
        e2 = spawn_block_vox(world, spawn_data);
    } else if (zox_has(prefab, RendererInstance)) {
        e2 = spawn_block_vox_instanced(world, spawn_data);
    } else {
        return;
    }
    link_node_VoxelNode(data->node, e2);

    /*zox_log("+ Placing Block [%s]: linked: [%i]", zox_get_name(data->block), data->node->linked)
    zox_log("   - local [%ix%ix%i] ", spawn_data.positionl.x, spawn_data.positionl.y, spawn_data.positionl.z)
    zox_log("   - global [%ix%ix%i] ", spawn_data.positionv.x, spawn_data.positionv.y, spawn_data.positionv.z)
    zox_log("   - real [%fx%fx%f] ", spawn_data.positionf.x, spawn_data.positionf.y, spawn_data.positionf.z)*/
    // spawn_line3D(world, spawn_data.positionf, float3_add(spawn_data.positionf, (float3) { 0, 2, 0 }), 2, 3);
}
