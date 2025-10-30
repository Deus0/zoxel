// sets the lines height based on frame times
// todo: use PlotData
void PlotLineSystem(iter *it) {
    const int graph_plot_height = 100;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_in(ChildIndex);
    zox_sys_out(LineLocalPosition2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parent);
        zox_sys_i(ChildIndex, index);
        zox_sys_o(LineLocalPosition2, position);

        if (!parent->value || !zox_has(parent->value, PlotDataDouble)) {
            zox_log_error("No PlotDataDouble found on parent");
            continue;
        }

        zox_geter(parent->value, PlotDataDouble, data);
        zox_geter_value(parent->value, PlotMin, double, min);
        zox_geter_value(parent->value, PlotMax, double, max);
        zox_geter_value(parent->value, LayoutSize, int2, parent_size);

        if (index->value >= data->length) {
            zox_log_error("index [%i] out of bounds (len: %i)", index->value, data->length);
            continue;
        }

        double line_max = max;
        if (max - min <= graph_plot_height) {
            // reset max bounds of our ms
            line_max = min + graph_plot_height;
        }
        double value = data->value[index->value];
        value /= line_max;
        position->value.w = (int) (value * parent_size.y);
    }
} zoxd_system2(PlotLineSystem);
