// A model bigger than a block (a bed: two blocks a side, a depth more, doc/plans/multi_block_vox.md)
//  - how many depths past a block's it is
static inline byte block_model_extra_depth(ecs* world, entity model) {
    const byte depth = zox_valid(model) && zox_has(model, MaxRenderDepth) ? zox_getv(model, MaxRenderDepth) : 0;
    return depth > block_depth_limits.y ? depth - block_depth_limits.y : 0;
}

//  - its render depth: the chunk's shifted by its extra depths, so it keeps a block's detail (the same voxels a block)
static inline byte block_model_render_depth(ecs* world, entity model, byte render_depth) {
    return render_depth <= block_depth_limits.y ? render_depth + block_model_extra_depth(world, model) : render_depth;
}

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
    // srand - pick the model randomly, off our position in world
    srand(positionf.x * positionf.z * positionf.y);
    entity model = get_model_variant(world, vox, rand());
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
    zox_setv(e, VoxelLocalPosition, positionl);
    zox_setv(e, BlockScale, scale);
    zox_set_link(world, e, ModelLink, model);
    zox_setv(e, MaxRenderDepth, mdepth);
    zox_setv(e, RenderDepth, block_model_render_depth(world, model, render_depth));
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
