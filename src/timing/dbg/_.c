const double zox_lag_cutoff = 1.5;

entity track_systems[64];
int track_system_count = 0;

void zox_track_system(ecs* world, entity system) {
    track_systems[track_system_count++] = system;
    zox_set(system, SystemDelta, { 0 });
}

void log_lagging_systems(ecs* world) {
    for (int i = 0; i < track_system_count; i++) {
        entity system = track_systems[i];

        if (!zox_valid(system) || !zox_has(system, SystemDelta)) {
            zox_log_error("System invalid at [%i]");
            continue;
        }

        zox_geter_value(system, SystemDelta, double, system_delta);

        if (system_delta < zox_lag_cutoff) {
            continue;
        }

        zox_logw("System [%s] Lagged [%fms]", zox_get_name(system), system_delta);
    }
}

/*#define zox_sys_end()\
    double system_delta_time = get_time_ms() - system_time_begin;\
    ecs_set(it->world, it->system, SystemDelta, { system_delta_time });*/
