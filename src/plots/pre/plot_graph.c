entity plot_time;
entity plot_time_system;
const float plot_line_thickness = 1.0f;

entity spawn_prefab_plot_graph(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("plot_graph");
    zox_add(e, Plot);
    zox_setv(e, PlotMin, 0);
    zox_setv(e, PlotMax, 0);
    zox_setv(e, PlotDataLink, 0);
    // zox_prefab_set(e, DoubleData, { 0 });
    // zox_prefab_set(e, PlotPaused, { 0 });
    return e;
}

// , byte is_label, byte label_line color text_fill, color text_outline,
entity spawn_plot_graph(
    ecs* world,
    entity parent,
    entity prefab,
    entity data_entity,
    int2 size,
    ushort lines_count,
    double start_value,
    color line_color)
{
    int line_margins = 8;   // x
    float line_spacing = ( size.x - line_margins * 2 ) / (float) (lines_count - 1);
    int lines_min_height = 0;
    int lines_max_height = size.y;
    float2 anchor = float2_half;
    int2 position = int2_zero;
    entity e = spawn_layout2(world, prefab, parent, position, size, anchor, 0);
    // Initialize the Curve Data
    zox_setv(e, PlotDataLink, data_entity);
    // DoubleData data = (DoubleData) { 0 };
    // initialize_DoubleData(&data, lines_count);
    for (ushort i = 0; i < lines_count; i++) {
        // data.value[i] = start_value;
        int position_x = line_margins + i * line_spacing;
        int2 start_position = (int2) { position_x, lines_min_height };
        int2 end_position = (int2) { position_x, lines_max_height };
        entity e2 = spawn_ui_line2_v2(
            world,
            e,
            start_position,
            end_position,
            line_color,
            plot_line_thickness,
            0,
            float2_zero,
            position,
            0);
        zox_set(e2, PlotLineIndex, { i });
        zox_set_parent(world, e2, e);
        zox_add(e2, PlotLine);
    }
    // zox_set_ptr(e, DoubleData, data);
    return e;
}
