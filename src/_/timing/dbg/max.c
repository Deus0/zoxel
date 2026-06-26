// NOTE: From Module, go through all systems and find biggest
zox_sys2(MaxSystemSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    entity max_system = 0;
    double max_delta = 0;
    for (int i = 0; i < zox_systems_count; i++) {
        entity system = zox_systems[i];
        if (!zox_valid(system)) {
            // zox_logw("System invalid at [%i]", i);
            continue;
        }
        double delta = zox_getv(system, MaxDoubleData); // SystemDeltaCache);
        if (delta > max_delta) {
            max_delta = delta;
            max_system = system;
        }
    }
    if (max_system && dbg_log) {
        zox_log("Biggest System Time [%s]:[%f]", zox_get_name(max_system), max_delta);
    }
    zox_sys_begin();
    zox_sys_out(SystemLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(SystemLink, max);
        max->value = max_system;
    }
} zox_sys_end(MaxSystemSystem);

// NOTE: From Module, go through all systems and find biggest
zox_sys2(MaxDataSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataDouble);
    zox_sys_out(MaxDoubleData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataDouble, data);
        zox_sys_o(MaxDoubleData, max);
        max->value = 0;
        for (int j = 0; j < data->length; j++) {
            if (data->value[j] > max->value) {
                max->value = data->value[j];
            }
        }
    }
} zox_sys_end(MaxDataSystem);

