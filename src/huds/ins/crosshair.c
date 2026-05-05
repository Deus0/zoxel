entity spawn_crosshair(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data) {
    zox_instance(element_data.prefab);
    zox_name("crosshair");
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    local_crosshair = e;
    return e;
}
