// MaxSystemTimeLabel
zox_sys2(MaxSystemTimeLabelSystem) {
    // Get the output
    zox_sys_world();
    /*entity maxe = max_systems_data;
    if (!zox_valid(maxe)) {
        zox_loge("Timing Module not valid.");
        return;
    }
    // entity max_system = zox_getv(maxe, SystemLink);*/
    if (!zox_valid(max_system)) {
        zox_loge("No Max System linked to Timing Module");
        return;
    }
    uint processes = zox_has(max_system, SystemProcessed) ?
        zox_getv(max_system, SystemProcessed) :
        0;
    double max_delta_per_curve = zox_getv(fps_curve, DoubleDataMax);
    double max_delta = zox_getv(max_system, DoubleDataMax);
    char output[1024];
    output[0] = '\0';
    sprintf(output, "Delta [%0.01fms] Max [%0.01fms]\n   - [%s]\n    - at [%0.01fms] x%i",
        (zox_delta_time * 1000),
        (max_delta_per_curve * 1000),
        zox_getn(max_system),
        max_delta,
        processes);
    /*sprintf(output, "Current Delta [%0.01fms]\nMax Frame Delta [%0.01fms]\n   - [%s]\n    - at [%0.01fms] x%i",
        (zox_delta_time * 1000),
        (max_delta_per_curve * 1000),
        zox_getn(max_system),
        max_delta,
        processes);*/
    uint text_length = strlen(output);
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, data);
        if (!is_zext_cut(
            data,
            output,
            text_length))
        {
            set_zext_cut(
                data,
                output,
                text_length);
            zox_add(e, Dirty);
        }
    }
} zox_sys_end(MaxSystemTimeLabelSystem);
