// Adds the latest time onto system and moves rest of list up
void add_double_to_samples(ecs *world, entity e, double value) {
    if (!zox_valid(e) || !zox_has(e, DoubleData)) {
        return;
    }
    /*if (zox_has(e, PlotPaused) && zox_getv(e, PlotPaused)) {
        zox_log("plot [%lu] is paused.", e);
        return;
    }*/
    zox_muter(e, DoubleData, data);
    if (!data || !data->length || !data->value) {
        return;
    }
    for (int i = 0; i < data->length - 1; i++) {
        data->value[i] = data->value[i + 1];
    }
    data->value[data->length - 1] = value;
}