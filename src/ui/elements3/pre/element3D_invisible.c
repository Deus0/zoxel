entity spawn_prefab_element3D_invisible(ecs *world) {
    zox_prefab()
    zox_prefab_name("element3D_invisible")
    zox_add_tag(e, Element3D)
    add_element3D_transform(world, e);
    zox_prefab_set(e, LocalPosition3D, { float3_zero })
    zox_prefab_set(e, LocalRotation3D, { quaternion_identity })
    return e;
}