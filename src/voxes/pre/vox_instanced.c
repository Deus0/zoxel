// TransformMatrix, RenderDisabled, InstanceLink
entity spawn_prefab_vox_instanced(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_instanced");
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    zox_prefab_set(e, InstanceLink, { 0 });  // directly whats rendering
    zox_prefab_set(e, ModelLink, { 0 });     // can be a model or model group
    zox_prefab_set(e, BlockScale, { vox_model_scale });
    zox_prefab_set(e, Bounds3D, { (float3) { 0.25f, 0.25f, 0.25f } });
    zox_prefab_set(e, Bounds3Dirty, { 0 });

    return e;
}
