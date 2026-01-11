entity2 spawn_slider(
    ecs *world,
    LayoutParentData canvas_data,
    LayoutParentData parent_data,
    ElementSpawnData element_data,
    SpawnSliderData slider_data,
    byte font_size,
    color font_fill,
    color font_outline
) {
    byte handle_width = 8 * ui_scale;
    byte font_thickness = ui_scale;

    zox_instance(element_data.prefab);
    zox_name("slider");
    zox_set(e, SliderLabel, { slider_data.name });
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
    Children children = (Children) { 0 };

    LayoutParentData new_parent_data = {
        .e = e,
        .size = element_data.size,
        .position = element_data.position_in_canvas,
    };

    // spawn handle
    int layout_x = -element_data.size.x / 2 + (int) (element_data.size.x * slider_data.value);
    entity handle = spawn_handle(
        world,
        canvas_data,
        new_parent_data,
        (ElementSpawnData) {
            .prefab = slider_data.prefab_handle,
            .layer = element_data.layer + 2,
            .position = (int2) { layout_x, 0 },
            .size = (int2) { handle_width, element_data.size.y },
            .anchor = float2_half,
            .render_disabled = element_data.render_disabled,
        }
    );
    zox_set(handle, SlideBounds, { slider_data.bounds })
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
            .font_resolution = font_size, // font_size * 2?
            .font_thickness = font_thickness,
            .font_outline_thickness = font_thickness,
            .font_fill_color = font_fill,
            .font_outline_color = font_outline,
        }
    };
    const entity text = spawn_zext(world, &text_data);
    add_to_Children(&children, text);

    // finish up
    zox_set_ptr(e, Children, children);
    // todo: with text label!
    // return, include the handle
    return (entity2) { e, handle };
}