const double zox_lag_cutoff = 2;

void add_system_log_components(ecs* world) {
    for (int i = 0; i < zox_systems_count; i++) {
        entity system = zox_systems[i];
        if (!zox_valid(system)) {
            zox_log_error("System invalid at [%i]", i);
            continue;
        }
        zox_set(system, SystemDelta, { 0 });
    }
}

zox_sys2(SystemDeltaLogSystem) {
    byte did_lag = 0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SystemDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SystemDelta, delta);

        if (delta->value < zox_lag_cutoff) {
            continue;
        }

        zox_logw("  - LAG [%s] [%fms]", zox_get_name(e), delta->value);
        did_lag = 1;
    }
    if (did_lag) {
        zox_logw("Total LAG [%fms]", delta_time * 1000.0);
    }
} zox_sys_end(SystemDeltaLogSystem);

// Reset our deltas
zox_sys2(SystemDeltaLogResetSystem) {
    zox_sys_begin();
    zox_sys_out(SystemDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SystemDelta, delta);
        delta->value = 0;
    }
} zox_sys_end(SystemDeltaLogResetSystem);

