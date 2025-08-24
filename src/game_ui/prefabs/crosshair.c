entity spawn_prefab_crosshair(ecs *world,
    const entity prefab)
{
    zox_prefab_child(prefab)
    zox_set(e, HitType, { 0 })
    zox_set(e, FrameCorner, { crosshair_corner })
    zox_set(e, OutlineThickness, { crosshair_thickness })
    zox_set(e, Color, { empty_color })
    zox_set(e, OutlineColor, { crosshair_air })
    return e;
}

entity spawn_crosshair(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    ElementSpawnData element_data
) {
    zox_instance(element_data.prefab);
    zox_name("crosshair");
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    local_crosshair = e;
    return e;
}
