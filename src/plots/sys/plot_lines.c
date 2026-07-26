// sets the lines height based on frame times
// todo: use PlotData
zox_sys2(PlotLineSystem) {
    const int graph_plot_height = 100;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlotLineIndex);
    zox_sys_out(LayoutLinePoints);
    // zox_sys_out(LineLocalPositionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlotLineIndex, index);
        zox_sys_o(LayoutLinePoints, position);
        /*zox_sys_o(LineLocalPositionDirty, dirty);
        if (dirty->value) {
            continue;
        }*/
        entity parent = zox_get_parent(world, e);
        if (!parent || !zox_has(parent, PlotDataLink)) {
            zox_loge("No PlotDataLink found on parent");
            continue;
        }
        entity data_entity = zox_getv(parent, PlotDataLink);
        if (!zox_valid(data_entity)) {
            zox_loge("Invalid data_entity");
            continue;
        }
        if (!zox_has(data_entity, DoubleData)) {
            zox_loge("No DoubleData found on data_entity [%s]", zox_get_name(data_entity));
            continue;
        }
        zox_geter(data_entity, DoubleData, data);
        zox_geter_value(parent, PlotMin, double, min);
        zox_geter_value(parent, PlotMax, double, max);
        zox_geter_value(parent, LayoutSize, int2, parent_size);
        if (index->value >= data->length) {
            zox_loge("index [%i] out of bounds (len: %i)", index->value, data->length);
            continue;
        }
        double line_max = max;
        if (max - min <= graph_plot_height) {
            // reset max bounds of our ms
            line_max = min + graph_plot_height;
        }
        double value = data->value[index->value];
        value /= line_max;
        position->end.y = (int) (value * parent_size.y);
        // dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(PlotLineSystem);
