// TODO: Seperate profiler from a simple graph_window that links to a Curve entity
color plot_colors[] = {
    (color) { 33, 183, 183, 255 },
    (color) { 163, 163, 163, 255 },
};

entity spawn_profiler(ecs* world, const char *header_label, entity canvas, int2 position, int2 size, float2 anchor) {
    if (!zox_valid(canvas)) {
        zox_loge("Invalid Canvas");
        return 0;
    }
    byte layer = game_overlay_layer + 3; // 3;
    byte plots_count = 1;
    byte plot_layer = layer + 1;
    int font_size = 28;
    entity3 e2 = spawn_window(world, prefab_window, prefab_body, "Profiler", canvas, position, size, anchor, NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_add_tag(e, EditorElement);
    zox_add_tag(e, Profiler);
    zox_set(e, PlotPaused, { 0 });
    zox_set(e, PlotDataDouble, { 0 });
    int2 plot_size = size;
    for (int i = 0; i < plots_count; i++) {
        entity plot_graph = spawn_plot_graph(world, canvas, body, position, size, prefab_plot_graph, plot_layer, plot_size, record_frames_count, 0, button_font_fill, button_font_outline, plot_colors[i], 1, i * 2);
        zox_set_parent(world, plot_graph, body);
        if (i == 0) {
            plot_time = plot_graph;
        } else {
            plot_time_system = plot_graph;
        }
    }
    return e;
}

entity spawn_profiler_canvas(ecs* world, entity canvas) {
    int2 position = { 0, 0 }; // - size.x / 2 - size.y / 2 };
    int2 size = { 380, 240 };
    float2 anchor = { 1.0f, 1.0f };
    return spawn_profiler(world, "Profiler", canvas, position, size, anchor);
}
