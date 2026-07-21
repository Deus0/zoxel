entity spawn_prefab_element3(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("element3");
    zox_add_tag(e, Element3);
    zox_add_tag(e, SingleMaterial);
    zox_prefab_set(e, FillColor, {{ 0, 255, 0, 255 }});
    zox_prefab_set(e, LocalPosition3D, { float3_zero });
    zox_prefab_set(e, LocalRotation3D, { quaternion_identity });
    add_ui_components_world(world, e, (float2) { 0.04f, 0.04f });
    return e;
}
