entity spawn_profiler(ecs* world, entity p, const char *header_label, int2 position, int2 size, float2 anchor, entity canvas, byte layer) {
    if (!zox_valid(canvas)) {
        zox_loge("Invalid Canvas");
        return 0;
    }
    byte plots_count = 1;
    color text_color = window_outline; // (color) { 255, 255, 255, 255 };
    color plot_colors[] = {
        window_outline, // (color) { 33, 133, 133, 255 },
        (color) { 163, 163, 163, 255 },
    };
    byte plot_layer = layer + 1;
    int font_size = 28;
    byte2 header_padding = byte2_single(12);
    float2 header_anchor = (float2) { 0.5f, 1.0f };
    int2 header_position = (int2) { 0, - font_size / 2 - header_padding.y / 2 };
    int2 header_size = (int2) { size.x, font_size + header_padding.y };
    byte is_header = 1;
    int2 canvas_size = zox_get_value(canvas, LayoutSize)
    byte is_close_button = 1;
    entity parent = canvas;
    zox_instance(p);
    zox_name("profiler");
    zox_add_tag(e, EditorElement);
    zox_add_tag(e, Profiler);
    initialize_element(world, e, parent, canvas, position, size, size, anchor, layer);
    set_window_bounds_to_canvas(world, e, canvas_size, size, anchor);
    // set texture
    zox_set(e, FrameCorner, { default_window_corner });
    zox_set(e, Color, { window_fill });
    zox_set(e, OutlineColor, { window_outline });
    if (is_header) {
        spawn_header(world, e, canvas, header_position, header_size, header_anchor, header_label, font_size, header_padding,  int2_zero, size, is_close_button, NULL, canvas_size);
    }
    int2 plot_size = size;
    plot_size.y -= header_size.y;
    for (int i = 0; i < plots_count; i++) {
        entity plot_graph = spawn_plot_graph(world, canvas, e, position, size, prefab_plot_graph, plot_layer, plot_size, record_frames_count, 0, text_color, plot_colors[i], 1, i * 2);
        zox_set_parent(world, plot_graph, e);
        if (i == 0) plot_time = plot_graph;
        else plot_time_system = plot_graph;
    }
    return e;
}

entity spawn_profiler_canvas(ecs* world, entity canvas) {
    byte layer = game_overlay_layer + 3; // 3;
    int2 test_window_size = { 380, 380 };
    int2 test_window_position = { - test_window_size.x / 2, test_window_size.y / 2 };
    float2 test_window_anchor = { 1.0f, 0.0f };
    return spawn_profiler(world, prefab_plot_window, "Profiler", test_window_position, test_window_size, test_window_anchor, canvas, layer);
}
