// TODO: Seperate profiler from a simple graph_window that links to a Curve entity
color plot_colors[] = {
    (color) { 33, 183, 183, 255 },
    (color) { 163, 163, 163, 255 },
};

entity spawn_profiler(ecs* world, entity canvas, const char *header_label, int2 position, int2 size, float2 anchor) {
    if (!zox_valid(canvas)) {
        zox_loge("Invalid Canvas");
        return 0;
    }
    // int font_size = 28;
    entity3 e2 = spawn_window(world, prefab_window, prefab_body, "Profiler", canvas, position, size, anchor, NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_add_tag(e, EditorElement);
    zox_add_tag(e, Profiler);
    zox_set(e, PlotPaused, { 0 });
    zox_set(e, DataDouble, { 0 });
    int2 plot_size = size;
    byte plots_count = 1;
    for (int i = 0; i < plots_count; i++) {
        entity e2 = spawn_plot_graph(world,body, prefab_plot_graph, plot_size, record_frames_count, 0, button_font_fill, button_font_outline, plot_colors[i], 1, i * 2);
        if (i == 0) {
            plot_time = e2;
        } else {
            plot_time_system = e2;
        }
    }
    // TODO: Spawn label here to show max system! like to timings module
    // entity module = zox_id(Timings);
    // entity max_label = spawn_text();
    {
        byte2 label_margins = (byte2) { 8 * ui_scale, 4 * ui_scale };   // x
        byte label_font_size = 8 * ui_scale;
        int2 position = (int2) { 0, - 3 * (14 + 4 * 2) };
        float2 position_anchor = (float2) { 0, 1.0f };
        entity e2 = spawn_text(world, prefab_text, e, position, position_anchor, label_font_size, zox_alignment_top_left, label_margins, "Max: ", button_font_fill, button_font_outline);
        // Link to module
        // Add Tag for it
    }
    /*for (int i = 0; i < zox_systems_count; i++) {
        entity system = zox_systems[i];
        if (!zox_valid(system)) {
            zox_log_error("System invalid at [%i]", i);
            continue;
        }
        if (!zox_has(system, DataDouble)) {
            continue;
        }
        entity e2 = spawn_plot_graph(world,body, prefab_plot_graph, plot_size, record_frames_count, 0, button_font_fill, button_font_outline, plot_colors[i], 1, i * 2);
        zox_set_parent(world, e2, body);
    }*/
    return e;
}

entity spawn_profiler_canvas(ecs* world, entity canvas) {
    int2 position = { 0, 0 }; // - size.x / 2 - size.y / 2 };
    int2 size = { 380, 240 };
    float2 anchor = { 1.0f, 1.0f };
    return spawn_profiler(world, canvas, "Profiler", position, size, anchor);
}
