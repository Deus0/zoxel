entity spawn_prefab_canvas3_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("canvas3_textured");
    zox_prefab_set(e, UIHolderLink, { 0 });
    zox_prefab_set(e, UITrail, {{ 0, 0.43f, 0 }});
    zox_prefab_set(e, FillColor, {{ 0, 255, 0, 255 }});
    zox_add_tag(e, SingleMaterial);
    return e;
}

// , entity camera, float3 offset
/*entity spawn_element3D(
    ecs *world,
    const entity prefab,
    const entity ui_holder,
    const entity canvas
) {
    const int2 pixel_size = (int2) { 32, 8 };
    zox_instance(prefab);
    zox_name("element3D");
    zox_set(e, UIHolderLink, { ui_holder });
    // zox_set(e, CameraLink, { camera })
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, LayoutSize, { pixel_size });
    zox_set(e, TextureSize, { pixel_size });
    return e;
}
*/
