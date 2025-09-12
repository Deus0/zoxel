entity spawn_vox_instanced(
    ecs *world,
    const entity prefab,
    const float3 position,
    const byte render_disabled,
    const byte render_depth,
    const float vox_scale
) {
    zox_instance(prefab);
    zox_name("vox_instanced");
    zox_set(e, Position3D, { position });
    zox_set(e, TransformMatrix, { float4x4_position(position) });
    zox_set(e, RenderDepth, { render_depth });
    zox_set(e, RenderDisabled, { render_disabled });
    zox_set(e, BlockScale, { vox_scale });
    return e;
}