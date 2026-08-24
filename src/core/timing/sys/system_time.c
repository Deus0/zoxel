zox_sys2(FpsCurveSystem) {
    zox_sys_world();
    if (!zox_valid(fps_curve)) {
        return;
    }
    zox_muter(fps_curve, DoubleData, data);
    for (int j = 0; j < data->length - 1; j++) {
        data->value[j] = data->value[j + 1];
    }
    data->value[data->length - 1] = zox_delta_time;
} zox_sys_end(FpsCurveSystem);
