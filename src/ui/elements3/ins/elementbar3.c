// used atm for statbar front bar
entity spawn_elementbar3D_front(ecs *world, entity prefab, entity parent, float3 offset, byte render_disabled) {
    zox_instance(prefab);
    zox_name("elementbar3D_front");
    // zox_set(e, ParentLink, { parent });
    zox_set_parent(world, e, parent);
    zox_set(e, LocalPosition3D, { offset });
    zox_set(e, RenderDisabled, { render_disabled });
    return e;
}

entity2 spawn_elementbar3(ecs *world, SpawnDataElementbar3D *data, Text3DData text_data, Zigel3DData zigel_data) {
    entity2 output = { 0 };
    zox_instance(data->backbar.prefab);
    output.x = e;
    zox_name("elementbar3D");
    zox_set(e, UIHolderLink, { data->ui_holder });
    zox_set(e, UITrail, { { 0, data->trail_offset, 0 } });
    zox_set(e, ElementBar, { data->percentage });
    zox_set(e, ElementBarSize, { statbar_front_mesh_scale });
    zox_set(e, RenderDisabled, { data->backbar.render_disabled });
    // Children children = (Children) { 0 };
    float3 frontbar_position = (float3) {
        0,
        0,
        element3D_depth_difference
    };
    entity frontbar = spawn_elementbar3D_front(
        world,
        data->frontbar.prefab,
        e,
        frontbar_position,
        data->frontbar.render_disabled
    );
    zox_set_parent(world, frontbar, e);
    //add_to_Children(&children, frontbar);
    if (text_data.prefab) {
        text_data.position = (float3) { 0, 0, element3D_depth_difference * 2 };
        zigel_data.position = text_data.position;
        text_data.parent = e;
        entity text = spawn_text3D(world, text_data, zigel_data);
        // add_to_Children(&children, text);
        zox_set_parent(world, text, e);
        zox_set_unique_name(text, "elementbar3D_text");
        output.y = text;
    }
    // zox_set_ptr(e, Children, children);
    return output;
}