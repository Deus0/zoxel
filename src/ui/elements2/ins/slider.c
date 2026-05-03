entity2 spawn_slider(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnSliderData slider_data, color fill, color handle_fill, byte font_size, color font_fill, color font_outline) {

    byte handle_width = slider_data.handle_width; // 8 * ui_scale;
    byte font_thickness = ui_scale;

    zox_instance(element_data.prefab);
    zox_name("slider");
    zox_set(e, SliderLabel, { slider_data.name });
    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { handle_fill });

    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);

    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }

    Children children = (Children) { 0 };

    LayoutParentData new_parent_data = {
        .e = e,
        .size = element_data.size,
        .position = element_data.position_in_canvas,
    };

    // spawn handle
    float percent = clampf(slider_data.value, 0, 1);
    int handle_position_x = - element_data.size.x / 2 + handle_width / 2 + (int) ((element_data.size.x - handle_width) * percent);

    // zox_log("handle_position_x %f to %i", slider_data.value, handle_position_x);

    ElementSpawnData handle_data = (ElementSpawnData) {
        .prefab = slider_data.prefab_handle,
        .layer = element_data.layer + 2,
        .position = (int2) { handle_position_x, 0 },
        .size = (int2) { handle_width, element_data.size.y },
        .anchor = float2_half,
        .render_disabled = element_data.render_disabled,
    };
    entity handle = spawn_handle(world, canvas_data, new_parent_data, handle_data, handle_fill, fill);

    zox_set(handle, SlideBounds, { slider_data.bounds });

    // zox_log("Slider Bounds %f:%f", slider_data.bounds.x, slider_data.bounds.y);

    add_to_Children(&children, handle);

    // # Slider Text #
    SpawnZext text_data = {
        .canvas = canvas_data,
        .parent = {
            .e = e,
            .position = element_data.position_in_canvas,
            .size = element_data.size,
        },
        .element = {
            .prefab = prefab_zext,
            .layer = element_data.layer + 1,
            .anchor = (float2) { 0.5f, 0.5f },
            .position = int2_zero,
            .render_disabled = element_data.render_disabled,
        },
        .zext = {
            .text = slider_data.name,
            .font_size = font_size,
            .font_resolution = font_size,
            .font_thickness = font_thickness,
            .font_outline_thickness = font_thickness,
            .font_fill_color = font_fill,
            .font_outline_color = font_outline,
        }
    };

    entity text = spawn_zext(world, text_data);
    add_to_Children(&children, text);

    // finish up
    zox_set_ptr(e, Children, children);

    // todo: with text label!
    // return, include the handle
    return (entity2) { e, handle };
}
