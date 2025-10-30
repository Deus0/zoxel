entity spawn_window(
    ecs *world,
    const char *header_label,
    int2 pixel_position,
    const int2 pixel_size,
    const float2 anchor,
    const entity canvas,
    const byte layer
) {
    const int2 canvas_size = zox_get_value(canvas, LayoutSize)
    const byte header_layer = layer + 1;
    const int font_size = 28;
    const int header_margins = 16;
    const entity parent = canvas;

    const int2 header_position = (int2) { 0, - font_size / 2 - header_margins / 2 };
    const int2 header_size = (int2) { pixel_size.x, font_size + header_margins};
    const float2 header_anchor = (float2) { 0.5f, 1.0f };
    zox_instance(prefab_window);
    zox_name("window");
    initialize_element(
        world,
        e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        pixel_size,
        anchor,
        layer
    );
    set_window_bounds_to_canvas(world, e, canvas_size, pixel_size, anchor);
    Children children = (Children) { 0, NULL };
    const entity header = spawn_header(world, e, canvas,
        header_position,
        header_size,
        header_anchor,
        header_label,
        font_size,
        header_margins,
        header_layer,
        int2_zero,
        pixel_size,
        1,
        canvas_size);
    add_to_Children(&children, header);
    zox_set_ptr(e, Children, children);
    return e;
}
