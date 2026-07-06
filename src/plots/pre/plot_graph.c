entity plot_time;
entity plot_time_system;
const float plot_line_thickness = 1.0f;

entity spawn_prefab_plot_graph(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("plot_graph");
    zox_add_tag(e, Plot);
    zox_set(e, PlotMin, { 0 });
    zox_set(e, PlotMax, { 0 });
    zox_prefab_set(e, DataDouble, { 0 });
    zox_prefab_set(e, PlotPaused, { 0 });
    return e;
}

// , byte is_label, byte label_line color text_fill, color text_outline,
entity spawn_plot_graph(ecs* world, entity parent, entity prefab, int2 size, int points_count, double start_value, color line_color) {
    int lines_count = record_frames_count;
    int line_margins = 8;   // x
    float line_spacing = ( size.x - line_margins * 2 ) / (float) (lines_count - 1);
    int lines_min_height = 0;
    int lines_max_height = size.y;
    // byte2 label_margins = (byte2) { 8 * ui_scale, 4 * ui_scale };   // x
    // byte label_font_size = 8 * ui_scale;
    float2 anchor = float2_half;
    int2 position = int2_zero;
    entity e = spawn_layout2(world, prefab, parent, position, size, anchor, 0);
    // Initialize the Curve Data
    DataDouble data = (DataDouble) { 0 };
    initialize_DataDouble(&data, points_count);
    for (int i = 0; i < points_count; i++) {
        data.value[i] = start_value;
    }
    zox_set_ptr(e, DataDouble, data);
    for (int i = 0; i < lines_count; i++) {
        int position_x = line_margins + i * line_spacing;
        int2 start_position = (int2) { position_x, lines_min_height };
        int2 end_position = (int2) { position_x, lines_max_height };
        entity e2 = spawn_ui_line2_v2(world, 0, e, start_position, end_position, line_color, plot_line_thickness, 0, float2_zero, position, 0);
        zox_set(e2, ChildIndex, { i });
        zox_set_parent(world, e2, e);
        zox_add_tag(e2, PlotLine);
    }
    return e;
}

    /*if (is_label) {
        int2 position = (int2) { 0, - label_line * (14 + 4 * 2) };
        float2 position_anchor = (float2) { 0, 1.0f };
        entity e2 = spawn_text(world, prefab_text, e, position, position_anchor, label_font_size, zox_alignment_top_left, label_margins, "", text_fill, text_outline);
        // entity e2 = spawn_text_old(world, text_data);
        // zox_set_parent(world, e2, e);
        zox_add_tag(e2, PlotLabel);
    }*/
