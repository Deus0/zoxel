zox_sys2(PlotLimitSystem) {
    zox_sys_begin();
    zox_sys_in(DataDouble);
    zox_sys_out(PlotMin);
    zox_sys_out(PlotMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DataDouble, data);
        zox_sys_o(PlotMin, min);
        zox_sys_o(PlotMax, max);

        if (!data->length) {
            continue;
        }

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
    }
} zox_sys_end(PlotLimitSystem);
