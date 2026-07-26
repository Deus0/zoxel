entity spawn_prefab_layout3(ecs *world) {
    zox_prefab();
    zox_prefab_name("layout3");
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    // zox_prefab_set(e, InitializeEntity, { 1 });
    zox_prefab_set(e, LayoutPosition, { int2_zero });
    zox_prefab_set(e, LayoutSize, { int2_zero });
    zox_prefab_set(e, Anchor, { float2_zero });
    // zox_prefab_set(e, CanvasPosition, { int2_zero });
    zox_prefab_set(e, Layer2D, { 0 });
    // zox_prefab_set(e, CanvasLink, { 0 });
    return e;
}
