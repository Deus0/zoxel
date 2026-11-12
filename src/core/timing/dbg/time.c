// NOTE: Adds the System Delta to the Curve
void system_time_add_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(SystemDeltaCache);
    zox_sys_out(DoubleData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SystemDeltaCache, time);
        zox_sys_o(DoubleData, data);
        if (!data->length || !data->value) {
            continue;
        }
        for (int j = 0; j < data->length - 1; j++) {
            data->value[j] = data->value[j + 1];
        }
        data->value[data->length - 1] = time->value;
    }
    zox_sys_on_end();
} zoxd_system(system_time_add_system);
