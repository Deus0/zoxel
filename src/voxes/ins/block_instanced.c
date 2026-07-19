entity spawn_block_vox_instanced(ecs *world, SpawnBlockVox data) {
    if (!zox_valid(data.vox)) {
        zox_loge("Invalid Block [%i] Vox for [spawn_block_vox_instanced]", data.block_index);
        return 0;
    }
    // NOTE: Picks a random Model based on position seed
    entity model = 0;
    if (zox_has(data.vox, ModelLinks)) {
        zox_geter(data.vox, ModelLinks, models);
        if (models->length) {
            // srand - pick the model randomly, off our position in world
            srand(data.positionf.x * data.positionf.z * data.positionf.y);
            byte model_index = rand() % models->length;
            model = models->value[model_index];
        } else {
            model = data.vox;
        }
    } else {
        model = data.vox;
    }
    if (!zox_valid(model)) {
        zox_log_error("Spawned BlockVoxInstance [%s::%lu] with Invalid Model [%lu] - block index [%i]", zox_get_name(data.prefab), data.prefab, data.vox, data.block_index);
        return 0;
    }
    if (!zox_has(model, MaxRenderDepth)) {
        zox_loge("Max Depth not on vox [%s]", zox_get_name(model));
        return 0;
    }
    byte mdepth = zox_getv(model, MaxRenderDepth);
    //byte ddepth = mdepth - block_vox_depth; // block_vox_depth;
    // float scale = 1.0f / (powers_of_two[ddepth]); // data.scale *
    float scale = 1.0f;
    // Spawn part
    zox_instance(data.prefab);
    zox_name("block_vox_instanced");
    zox_set(e, BlockIndex, { data.block_index });
    zox_set(e, BlockScale, { data.scale });
    zox_set(e, ModelLink, { model });
    zox_set(e, MaxRenderDepth, { mdepth });
    zox_set(e, RenderDepth, { data.render_depth });
    zox_set(e, RenderDisabled, { data.render_disabled });
    zox_set(e, Position3D, { data.positionf });
    zox_set(e, Scale1D, { scale });
    zox_set(e, TransformMatrix, { float4x4_position_scale(data.positionf, scale) });
    //zox_set(e, TransformMatrix, { float4x4_position(positionf) });
    // zox_set(e, TransformMatrix, { float4x4_transform_scale(positionf, quaternion_identity, 1) });
    // zox_set(e, TransformMatrix, { float4x4_transform(positionf, quaternion_identity) });
    return e;
}
