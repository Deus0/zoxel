// NOTE: Goes through all Doubles on array and find biggest
zox_sys2(MaxDataSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DoubleData);
    zox_sys_out(DoubleDataMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DoubleData, data);
        zox_sys_o(DoubleDataMax, max);
        max->value = 0;
        if (!data->value || !data->length) {
            zox_logw("DoubleData not intiialized for [%s]", zox_get_name(e));
            continue;
        }
        for (int j = 0; j < data->length; j++) {
            if (data->value[j] > max->value) {
                max->value = data->value[j];
            }
        }
    }
} zox_sys_end(MaxDataSystem);
