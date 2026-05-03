entity spawn_handle(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, color fill, color outline) {

    zox_instance(element_data.prefab);
    zox_name("handle");

    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { outline });

    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }

    // Constrains within slider
    zox_set(e, LayoutConstraints, { (int4) {
        -(parent_data.size.x / 2) + (element_data.size.x / 2),
        (parent_data.size.x / 2) - (element_data.size.x / 2),
        0, 0} });

    return e;
}
