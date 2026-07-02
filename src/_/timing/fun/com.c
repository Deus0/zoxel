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
        // zox_set(system, DataDouble, { 0 });
        uint length = sizeof(double) * record_frames_count;
        double* values = malloc(length);
        for (int i = 0; i < record_frames_count; i++) {
            values[i] = 0;
        }
        zox_set(system, DataDouble, { .value = values, .length = record_frames_count });
        // zox_muter(system, DataDouble, data);
        // initialize_DataDouble(data, record_frames_count);
        /*DataDouble data = { };
        initialize_DataDouble(&data, record_frames_count);
        zox_set_ptr(system, DataDouble, data);*/
    }
}
