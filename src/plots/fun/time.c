extern entity plot_time;
extern entity plot_time_system;

// Adds the latest time onto system and moves rest of list up
void add_plot_data_double(ecs *world, entity e, double value) {
    if (!zox_valid(e) || !zox_has(e, DataDouble)) {
        return;
    }
    if (zox_has(e, PlotPaused) && zox_getv(e, PlotPaused)) {
        zox_log("plot [%lu] is paused.", e);
        return;
    }
    zox_muter(e, DataDouble, data);
    if (!data || !data->length || !data->value) {
        return;
    }
    for (int i = 0; i < data->length - 1; i++) {
        data->value[i] = data->value[i + 1];
    }
    data->value[data->length - 1] = value;
}

void add_plot_data_time(ecs *world, double value_s) {
    add_plot_data_double(world, plot_time, value_s * 1000);
}

void add_plot_data_time_system(ecs *world, double value_ms) {
    add_plot_data_double(world, plot_time_system, value_ms);
}
