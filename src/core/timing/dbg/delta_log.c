const byte is_log_totals = 0;

// used to sort times in ui
typedef struct {
    entity e;
    double value;
} system_delta_entry;

static int cmp_system_delta_desc(const void *a, const void *b) {
    const system_delta_entry *da = a;
    const system_delta_entry *db = b;
    if (da->value < db->value) return 1;
    if (da->value > db->value) return -1;
    return 0;
}

int system_debug_start = -1;

// NOTE: Now we use MaxDoubleData, the max delta over the time period
uint debug_ui_system_times(ecs *world, entity player, char *buffer, uint size, uint index) {
    uint display_count = system_times_display_count;
    // MaxDoubleData vs SystemDeltaCache
    // - SystemDeltaCache is per Frame
    // - MaxDoubleData is per Graph
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = zox_id(MaxDoubleData) } // ecs_id(SystemDeltaCache) }
        }
    });
    int count = 0;
    // First pass: count
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        count += it.count;
    }
    index += snprintf(buffer + index, size - index, "System Times [%i]\n", count);
    index += snprintf(buffer + index, size - index, "- Frame [%.2fms]\n", zox_delta_time * 1000);
    if (count == 0) {
        // ecs_query_fini(q);
        return index;
    }
    system_delta_entry *entries = malloc(sizeof(system_delta_entry) * count);
    /* Second pass: collect */
    int idx = 0;
    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        // SystemDeltaCache *deltas = ecs_field(&it, SystemDeltaCache, 0);
        MaxDoubleData *deltas = ecs_field(&it, MaxDoubleData, 0);
        for (int i = 0; i < it.count; i++) {
            entries[idx].e = it.entities[i];
            entries[idx].value = deltas[i].value;
            idx++;
        }
    }
    uint start = system_debug_start * 10;
    qsort(entries, count, sizeof(system_delta_entry), cmp_system_delta_desc);
    int top = int_min(count, start + display_count); // count < display_count ? count : display_count;
    for (int i = start; i < top; i++) {
        system_delta_entry entry = entries[i];
        entity e =  entry.e;
        index += snprintf(
            buffer + index,
            size - index,
            "  %.2fms %s",
            entry.value,
            // i + 1,  %2d.
            zox_get_name(e)
        );
        // Add process data
        /*if (zox_has(e, SystemProcessedCache)) {
            zox_geter_value(e, SystemProcessedCache, int, process_count);
            index += snprintf(buffer + index, size - index,"  p [%i]", process_count);
        }*/
        index += snprintf(buffer + index, size - index, "\n");
    }
    free(entries);
    // ecs_query_fini(q);
    return index;
}

// NOTE: zox_delta_time is on main thread
//      These system deltas are max for any thread
zox_sys2(SystemDeltaLogSystem) {
    byte did_lag = 0;
    double total = 0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SystemDeltaCache);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SystemDeltaCache, delta);
        total += delta->value;
        double cutoff = zox_lag_cutoff;
        if (zox_has(e, SystemDeltaMax)) {
            cutoff = zox_getv(e, SystemDeltaMax);
        }
        if (delta->value < cutoff) {
            continue;
        }
#ifdef zox_debug
        if (zox_log_lags) {
            zox_logw("Lag Detected -> %s: [%fms]", zox_get_name(e), delta->value);
        }
#endif
        did_lag = 1;
    }
    if (did_lag && is_log_totals) {
        zox_log("# Delta Totals #");
        zox_log("   - Logged [%fms]", total);
        zox_log("   - Zoxxed [%fms]", delta_time * 1000.0);
        zox_log("# # # # # # # # #");
    }
} zox_sys_end(SystemDeltaLogSystem);
