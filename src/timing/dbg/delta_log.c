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

        double cutoff = zox_lag_cutoff;
        if (zox_has(e, SystemDeltaMax)) {
            cutoff = zox_gett_value(e, SystemDeltaMax);
        }

        if (delta->value < cutoff) {
            continue;
        }

        zox_logw("  - LAG [%s] [%fms]", zox_get_name(e), delta->value);
        did_lag = 1;
    }
    if (did_lag) {
        zox_logw("[LAG] Frame Time [%fms]", delta_time * 1000.0);
    }
} zox_sys_end(SystemDeltaLogSystem);
