// TODO: Seperate profiler from a simple graph_window that links to a Curve entity
color plot_colors[] = {
    (color) { 33, 183, 183, 255 },
    (color) { 163, 163, 163, 255 },
};

entity spawn_profiler(
    ecs* world,
    entity canvas,
    const char* header_label,
    int2 position,
    int2 size,
    float2 anchor)
{
    if (!zox_valid(canvas)) {
        zox_loge("Invalid Canvas");
        return 0;
    }
    // NOTE: Uses PlotMin and PlotMax
    entity curve = frame_times_samples;
    // entity curve = fps_curve; // frame_times_samples;
    ushort lines_count = record_frames_count;
    byte plots_count = 1;
    double start_value = 32;
    byte header_font_size = 8 * ui_scale;
    byte label_font_size = 4 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e2 = spawn_window(
        world,
        prefab_window,
        prefab_body,
        "Frames",
        canvas,
        position,
        size,
        anchor,
        header_font_size,
        header_padding,
        NULL);
    entity e = e2.x;
    entity header = e2.y;
    entity body = e2.z;
    zox_add(e, Profiler);
    set_editor_window(world, e);
    zox_setv(body, FillColor, editor_window_fill);
    zox_setv(body, OutlineColor, editor_window_outline);
    zox_setv(header, FillColor, editor_header_fill);
    zox_setv(header, OutlineColor, editor_header_outline);
    zox_setv(header, ElementFillColor, editor_header_fill);
    zox_setv(header, ElementOutlineColor, editor_header_outline);
    zox_setv(e, PlotPaused, 0);
    zox_setv(e, DoubleData, 0);
    int2 plot_size = size;
    for (int i = 0; i < plots_count; i++) {
        entity e2 = spawn_plot_graph(
            world,
            body,
            prefab_plot_graph,
            curve,
            plot_size,
            lines_count,
            start_value,
            plot_colors[i]);
        if (i == 0) {
            plot_time = e2;
        } else {
            plot_time_system = e2;
        }
    }
    // TODO: Spawn label here to show max system! like to timings module
    {
        byte2 label_margins = (byte2) { 2 * ui_scale, 2 * ui_scale };   // x
        // NOTE: Positions below header
        int2 position = (int2) { 0, - 2 * (14 + 4 * 2) };
        float2 position_anchor = (float2) { 0, 1.0f };
        entity e2 = spawn_text(
            world,
            prefab_text,
            e,
            position,
            position_anchor,
            label_font_size,
            zox_alignment_top_left,
            label_margins,
            "",
            button_font_fill,
            button_font_outline);
        zox_add(e2, MaxSystemTimeLabel);
    }
    return e;
}

entity spawn_profiler_canvas(ecs* world, entity canvas) {
    int2 position = { 0, 0 }; // - size.x / 2 - size.y / 2 };
    int2 size = { 420, 280 };
    float2 anchor = { 1.0f, 0.0f };
    return spawn_profiler(
        world,
        canvas,
        "Profiler",
        position,
        size,
        anchor);
}
