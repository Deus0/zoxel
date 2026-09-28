entity spawn_block_vox_instanced(
    ecs* world,
    entity prefab,
    entity block,
    entity vox,
    byte block_index,
    byte render_depth,
    byte render_disabled,
    byte3 positionl,
    int3 positionv,
    float3 positionf)
{
    if (!zox_valid(vox)) {
        zox_loge("Invalid Block [vox] at [%s] - block [%s] at [%i] Vox for [spawn_block_vox_instanced]",
            zox_getn(prefab),
            zox_getn(block),
            block_index);
        return 0;
    }
    // NOTE: Picks a random Model based on position seed
    entity model = 0;
    if (zox_has(vox, ModelLinks)) {
        zox_geter(vox, ModelLinks, models);
        if (models->length) {
            // srand - pick the model randomly, off our position in world
            srand(positionf.x * positionf.z * positionf.y);
            byte model_index = rand() % models->length;
            model = models->value[model_index];
        } else {
            model = vox;
        }
    } else {
        model = vox;
    }
    if (!zox_valid(model)) {
        zox_loge("Spawned BlockVoxInstance [%s::%lu] with Invalid Model [%lu] - block index [%i]",
            zox_get_name(prefab),
            prefab,
            vox,
            block_index);
        return 0;
    }
    if (!zox_has(model, MaxRenderDepth)) {
        zox_loge("Max Depth not on vox [%s]",
            zox_getn(model));
        return 0;
    }
    byte mdepth = zox_getv(model, MaxRenderDepth);
    //byte ddepth = mdepth - block_depth; // block_depth;
    // float scale = 1.0f / (powers_of_two[ddepth]); // scale *
    float scale = 1.0f;
    // Spawn part
    zox_instance(prefab);
    zox_name("block_vox_instanced");
    zox_setv(e, BlockIndex, block_index);
    zox_setv(e, BlockScale, scale);
    zox_setv(e, ModelLink, model);
    zox_setv(e, MaxRenderDepth, mdepth);
    zox_setv(e, RenderDepth, render_depth);
    zox_setv(e, RenderDisabled, render_disabled);
    zox_setv(e, Position3D, positionf);
    zox_setv(e, Scale1, scale);
    zox_setv(e, TransformMatrix,
        float4x4_position_scale(positionf, scale));
    //zox_set(e, TransformMatrix, { float4x4_position(positionf) });
    // zox_set(e, TransformMatrix, { float4x4_transform_scale(positionf, quaternion_identity, 1) });
    // zox_set(e, TransformMatrix, { float4x4_transform(positionf, quaternion_identity) });
    return e;
}
