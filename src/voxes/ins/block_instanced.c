entity spawn_block_vox_instanced(
    ecs *world,
    const SpawnBlockVox data
) {

    zox_instance(data.prefab);
    zox_name("block_vox_instanced");

    // Voxel
    zox_set(e, BlockIndex, { data.block_index });
    zox_set(e, BlockScale, { data.scale });

    // Transform
    zox_set(e, Position3D, { data.positionf });
    zox_set(e, TransformMatrix, { float4x4_position(data.positionf) });

    // Render
    zox_set(e, RenderDepth, { data.render_depth });
    zox_set(e, RenderDisabled, { data.render_disabled });
    // set a random one if contains variants

    entity model = 0;
    if (zox_valid(data.vox)) {
        // Model Group
        if (zox_has(data.vox, ModelLinks)) {
            // zox_log("+ model group detected [%s]", zox_get_name(data.vox))
            zox_geter(data.vox, ModelLinks, models);
            if (models->length) {
                // srand - pick the model randomly, off our position in world
                srand(data.positionf.x * data.positionf.z * data.positionf.y);
                model = models->value[rand() % (models->length)];
                // zox_log("   + picked [%s]", zox_get_name(model))
            } else {
                model = data.vox;
            }
        } else {
            model = data.vox;
        }
    }

    if (zox_valid(model) && zox_has(model, MaxRenderDepth)) {
        zox_set(e, ModelLink, { model });
        zox_set(e, MaxRenderDepth, { zox_gett_value(model, MaxRenderDepth) });
    } else {
        zox_log_error("Spawned BlockVoxInstance [%s::%lu] with Invalid Model [%lu] - block index [%i]", zox_get_name(data.prefab), data.prefab, data.vox, data.block_index);
    }

    /*zox_log("+ new block vox instance [%fx%fx%f] s[%f] d[%i]",
        data.positionf.x, data.positionf.y, data.positionf.z,
        data.scale,
        data.render_depth);*/

    return e;
}