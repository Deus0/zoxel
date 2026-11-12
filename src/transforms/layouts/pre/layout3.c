entity spawn_prefab_layout3(ecs *world) {
    zox_prefab();
    zox_prefab_name("layout3");
    zox_set(e, TransformMatrix, { float4x4_identity });
    zox_set(e, Position3D, { float3_zero });
    zox_set(e, Rotation3D, { float4_identity });
    zox_set(e, Anchor, { float2_zero });
    zox_set(e, Layer, { 0 });
    zox_set(e, LayoutPosition, { int2_zero });
    zox_set(e, LayoutSize, { int2_zero });
    zox_add(e, LayoutPositionDirty);
    zox_add(e, LayoutSizeDirty);
    return e;
}

entity spawn_prefab_layout3_child(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("layout3_child");
    zox_set(e, LocalPosition3D, { float3_zero });
    zox_set(e, LocalRotation3D, { quaternion_identity });
    return e;
}
