// TransformMatrix, RenderDisabled, InstanceLink (set by lod_instance_system)
entity spawn_prefab_vox_instanced(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_instanced");
    zox_setv(e, RenderDepth, 0);
     //zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    zox_setv(e, BlockScale, vox_model_scale);
    zox_setv(e, Bounds3D, float3_single(0.25f));
    zox_setv(e, Bounds3Dirty, 0);
    zox_setv(e, RenderDisabled, 0);
    return e;
}
