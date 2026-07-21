entity spawn_prefab_canvas3(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("canvas3");
    zox_add_tag(e, Element3);
    //add_element3D_transform(world, e);
    add_ui_components_world(world, e, (float2) { 0.04f, 0.04f });
    zox_add_tag(e, ElementBillboard);
    zox_prefab_set(e, CameraLink, { 0 });
    return e;
}
