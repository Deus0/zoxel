#include "delta_log.c"
#include "reset.c"

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

void add_system_process_counter(ecs* world, entity e) {
    zox_add(e, SystemProcessed);
    zox_add(e, SystemProcessedCache);
}

void define_systems_timing_debug(ecs* world) {
    zox_system(
        SystemProcessedResetSystem,
        EcsOnLoad,
        [out] timing.SystemProcessed,
        [out] timing.SystemProcessedCache
    );

    zox_system(
        SystemDeltaLogResetSystem,
        EcsOnLoad,
        [out] timing.SystemDelta,
        [out] timing.SystemDeltaCache
    );

    zox_system_1(
        SystemDeltaLogSystem,
        EcsOnStore,
        [in] timing.SystemDeltaCache
    );
}