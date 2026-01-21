const double zox_lag_cutoff = 6;
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
    }
}

uint debug_ui_system_times(ecs *world, entity player, char *buffer, uint size, uint index) {

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(SystemDeltaCache) }
        }
    });

    int count = 0;

    /* First pass: count */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        count += it.count;
    }

    index += snprintf(
        buffer + index,
        size - index,
        "Systems (%i) [%fms]\n",
        count,
        zox_delta_time * 1000
    );

    if (count == 0) {
        // ecs_query_fini(q);
        return index;
    }

    system_delta_entry *entries =
        malloc(sizeof(system_delta_entry) * count);

    /* Second pass: collect */
    int idx = 0;
    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        SystemDeltaCache *deltas = ecs_field(&it, SystemDeltaCache, 0);
        for (int i = 0; i < it.count; i++) {
            entries[idx].e = it.entities[i];
            entries[idx].value = deltas[i].value;
            idx++;
        }
    }

    qsort(entries, count, sizeof(system_delta_entry), cmp_system_delta_desc);

    int top = count < 10 ? count : 10;
    for (int i = 0; i < top; i++) {
        index += snprintf(
            buffer + index,
            size - index,
            "  %2d. %-32s %8.3f ms\n",
            i + 1,
            ecs_get_name(world, entries[i].e),
            entries[i].value
        );
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
            cutoff = zox_gett_value(e, SystemDeltaMax);
        }

        if (delta->value < cutoff) {
            continue;
        }

        zox_logw("Lag Detected -> %s: [%fms]", zox_get_name(e), delta->value);
        did_lag = 1;
    }
    if (did_lag && is_log_totals) {
        zox_log("# Delta Totals #");
        zox_log("   - Logged [%fms]", total);
        zox_log("   - Zoxxed [%fms]", delta_time * 1000.0);
        zox_log("# # # # # # # # #");
    }
} zox_sys_end(SystemDeltaLogSystem);
