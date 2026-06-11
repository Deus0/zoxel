entity plot_time;
entity plot_time_system;
const float plot_line_thickness = 1.0f;

entity spawn_prefab_plot_graph(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("plot_graph");
    zox_add_tag(e, Plot);
    zox_set(e, PlotMin, { 0 });
    zox_set(e, PlotMax, { 0 });
    zox_prefab_set(e, PlotDataDouble, { 0 });
    zox_prefab_set(e, PlotPaused, { 0 });
    return e;
}


entity spawn_plot_graph(ecs* world, entity canvas, entity parent, int2 parent_position, int2 parent_size, entity prefab, byte layer, int2 size, int points_count, double start_value, color text_fill, color text_outline, color line_color, byte is_label, byte label_line) {
    int lines_count = record_frames_count;
    byte2 label_margins = (byte2) { 16, 8 };   // x
    int line_margins = 8;   // x
    float line_spacing = ( size.x - line_margins * 2 ) / (float) (lines_count - 1);
    int lines_min_height = 0;
    int lines_max_height = size.y;
    byte label_font_size = 16;
    float2 anchor = float2_half;
    int2 position = int2_zero;
    entity e = spawn_layout2(world, prefab, parent, position, size, anchor, layer);
    PlotDataDouble data = (PlotDataDouble) { 0 };
    initialize_PlotDataDouble(&data, points_count);
    for (int i = 0; i < points_count; i++) {
        data.value[i] = start_value;
    }
    zox_set_ptr(e, PlotDataDouble, data);
    if (is_label) {
        SpawnZext text_data = {
            .canvas = {
                .e = canvas,
                // .size = zox_gett_value(canvas, LayoutSize)
            },
            .parent = {
                .e = e,
                // .position = parent_position,
                // .size = parent_size
            },
            .element = {
                .prefab = prefab_text,
                .position = (int2) { 0, - label_line * (14 + 4 * 2) },
                // .layer = layer,
                .anchor = (float2) { 0, 1.0f },
            },
            .zext = {
                .font_resolution = label_font_size,
                .font_size = label_font_size,
                .font_thickness = 1,
                .alignment = zox_alignment_top_left,
                .margins = label_margins,
                .font_fill_color = text_fill,
                .font_outline_color = text_outline
            }
        };
        entity e2 = spawn_text_old(world, text_data);
        zox_set_parent(world, e2, e);
        zox_add_tag(e2, PlotLabel);
    }
    // our plot here
    for (int i = 0; i < lines_count; i++) {
        int position_x = line_margins + i * line_spacing;
        int2 start_position = (int2) { position_x, lines_min_height };
        int2 end_position = (int2) { position_x, lines_max_height };
        entity e2 = spawn_ui_line2_v2(world, canvas, e, start_position, end_position, line_color, plot_line_thickness, 0, float2_zero, position, layer);
        zox_set(e2, ChildIndex, { i });
        zox_set_parent(world, e2, e);
        zox_add_tag(e2, PlotLine);
    }
    return e;
}
