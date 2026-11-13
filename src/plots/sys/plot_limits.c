void plot_limit_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PlotMin);
    zox_sys_out(PlotMax);
    for (int i = 0; i < it->count; i++) {
        entity link = zox_get_link(world, it->entities[i], PlotDataLink);
        zox_sys_o(PlotMin, min);
        zox_sys_o(PlotMax, max);
        if (!zox_valid(link) ||
            !zox_has(link, DoubleData))
        {
            zox_loge("No DoubleData found on data->value");
            continue;
        }
        const DoubleData* data = zox_get(link, DoubleData);
        if (!data->length) {
            continue;
        }
        // max->value = zox_getv(link, DoubleDataMax);

        double min_ = 99999;
        double max_ = -99999;
        for (int j = 0; j < data->length; j++) {
            const double value = data->value[j];
            if (value <= min_) {
                min_ = value;
            } else if (value >= max_) {
                max_ = value;
            }
        }
        min->value = min_;
        max->value = max_;

        max->value *= 2;
    }
    zox_sys_on_end();
} zoxd_system(plot_limit_system);
