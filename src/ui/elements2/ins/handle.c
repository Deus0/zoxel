entity spawn_handle(ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data)
{
    zox_instance(element_data.prefab);
    zox_name("handle");
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent_data,
        element_data
    );
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    zox_set(e, LayoutConstraints, { (int4) {
        -(parent_data.size.x / 2) + (element_data.size.x / 2),
        (parent_data.size.x / 2) - (element_data.size.x / 2),
        0,
        0
    } });
    return e;
}