#include "delta_log.c"

/*void log_lagging_systems(ecs* world) {
    for (int i = 0; i < zox_systems_count; i++) {
        entity system = zox_systems[i];

        if (!zox_valid(system) || !zox_has(system, SystemDelta)) {
            zox_log_error("System invalid at [%i]", i);
            continue;
        }

        zox_geter_value(system, SystemDelta, double, system_delta);

        if (system_delta < zox_lag_cutoff) {
            continue;
        }

        zox_logw("System [%s] Lagged [%fms]", zox_get_name(system), system_delta);
    }
}*/