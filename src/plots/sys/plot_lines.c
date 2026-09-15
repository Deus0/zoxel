// sets the lines height based on frame times
// todo: use PlotData
void plot_line_system(iter* it) {
    const int graph_plot_height = 100;
    entity cached_parent;
    int2 parent_size;
    double min;
    double max;
    double line_max;
    const DoubleData* data;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlotLineIndex);
    zox_sys_out(LayoutLinePoints);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlotLineIndex, index);
        zox_sys_o(LayoutLinePoints, position);
        entity parent = zox_get_parent(world, e);
        if (cached_parent != parent) {
            if (!zox_valid(parent) ||
                !zox_has(parent, PlotDataLink)) {
                zox_loge("No PlotDataLink found on parent");
                continue;
            }
            entity data_entity = zox_getv(parent, PlotDataLink);
            if (!zox_has(data_entity, DoubleData)) {
                zox_loge("Invalid [data_entity] [%s]",
                    zox_getn(data_entity));
                continue;
            }
            cached_parent = parent;
            parent_size = zox_getv(parent, LayoutSize);
            min = zox_getv(parent, PlotMin);
            max = zox_getv(parent, PlotMax);
            if (max < zox_delta_time) {
                max = zox_delta_time;
            }
            data = zox_get(data_entity, DoubleData);
            line_max = max;
            if (max - min <= graph_plot_height) {
                line_max = min + graph_plot_height;
            }
        }
        if (index->value >= data->length) {
            zox_loge("index [%i] out of bounds (len: %i)",
                index->value,
                data->length);
            continue;
        }
        double value = data->value[index->value];
        double range = line_max - min;
        if (range > 0.0) {
            value = (value - min) / range;
        } else {
            value = 0.0;
        }
        position->end.y = (int)(value * parent_size.y);
    }
} zoxd_system(plot_line_system);


/*double line_max = max;
if (max - min <= graph_plot_height) {
    // reset max bounds of our ms
    line_max = min + graph_plot_height;
}
double value = data->value[index->value];
value /= line_max;
position->end.y = (int) (value * parent_size.y);
// dirty->value = zox_dirty_trigger;*/
