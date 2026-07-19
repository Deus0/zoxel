// When defining system
void add_system_log_components(ecs* world) {
    for (int i = 0; i < zox_systems_count; i++) {
        entity system = zox_systems[i];
        if (!zox_valid(system)) {
            zox_log_error("System invalid at [%i]", i);
            continue;
        }
        zox_set(system, SystemDelta, { 0 });
        zox_set(system, SystemDeltaCache, { 0 });
        zox_set(system, MaxDoubleData, { 0 });
        // zox_set(system, DoubleData, { 0 });
        uint length = sizeof(double) * record_frames_count;
        double* values = malloc(length);
        memset(values, 0, length);
        zox_set(system, DoubleData, { .value = values, .length = record_frames_count });
    }
}
