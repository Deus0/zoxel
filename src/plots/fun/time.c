extern entity plot_time;
extern entity plot_time_system;

void add_plot_data_double(
    ecs *world,
    const entity e,
    const double value
) {
    if (!zox_valid(e) || !zox_has(e, PlotDataDouble)) {
        return;
    }
    if (zox_has(e, PlotPaused) && zox_gett_value(e, PlotPaused)) {
        zox_log("plot [%lu] is paused.", e);
        return;
    }
    zox_muter(e, PlotDataDouble, data);
    if (!data || !data->length || !data->value) {
        return;
    }
    for (int i = 0; i < data->length - 1; i++) {
        data->value[i] = data->value[i + 1];
    }
    data->value[data->length - 1] = value;
}

void add_plot_data_time(
    ecs *world,
    const double value_s
) {
    add_plot_data_double(world, plot_time, value_s * 1000);
}

void add_plot_data_time_system(
    ecs *world,
    const double value_ms
) {
    add_plot_data_double(world, plot_time_system, value_ms);
}
