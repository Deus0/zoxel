// every system's total time spent (seconds), measured by flecs (ecs_measure_system_time in init.c)
//  - for profilers: the difference between two frames is a system's time that frame
//  - multi threaded systems' times add up every worker's (cpu time), divide by zox_worker_count for real time
typedef void (*zox_system_time_fun)(ecs* world, entity system, double seconds, byte multi_threaded, void* ctx);

void zox_each_system_time(ecs* world, zox_system_time_fun fun, void* ctx) {
    ecs_iter_t it = ecs_each_id(world, EcsSystem);
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const ecs_system_t* system = ecs_system_get(world, it.entities[i]);
            if (system) {
                fun(world, it.entities[i], system->time_spent, system->multi_threaded, ctx);
            }
        }
    }
}


// the world's running totals (seconds): its frames, merging deferred commands, notifying observers
typedef struct {
    double frame;
    double merge;
    double emit;
    // commands (deferred, applied in merges): adds, removes, deletes, sets
    int64_t adds;
    int64_t removes;
    int64_t deletes;
    int64_t sets;
    int64_t observers;      // observer invocations (flecs' own too)
    int64_t tables;         // tables created
} ZoxWorldTimes;

ZoxWorldTimes zox_world_times(ecs* world) {
    const ecs_world_info_t* info = ecs_get_world_info(world);
    return (ZoxWorldTimes) { info->frame_time_total, info->merge_time_total, info->emit_time_total,
        info->cmd.add_count, info->cmd.remove_count, info->cmd.delete_count, info->cmd.set_count + info->cmd.ensure_count,
        info->observers_ran_total, info->table_create_total };
}

// threads running multi threaded systems
int zox_worker_count(ecs* world) {
    const int count = ecs_get_stage_count(world);
    return count > 0 ? count : 1;
}

// Table families: tables grouped by their type with each pair's target as *, i.e. (ChildOf,*)
//  - a family with many tables is fragmenting: a pair (ChildOf, IsA, a link) with many targets
//  - fills families (signatures) and counts, most tables first, returns how many families, total tables
#define zox_table_family_length 2048

static int zox_compare_pointers(const void* a, const void* b) {
    const uintptr_t x = *(const uintptr_t*) a, y = *(const uintptr_t*) b;
    return x < y ? -1 : x > y ? 1 : 0;
}

// a table's type, pair targets replaced by *
static inline void zox_table_family(ecs* world, ecs_table_t* table, char* out) {
    char* type = ecs_table_str(world, table);
    int o = 0;
    byte in_pair = 0, after_comma = 0;
    for (int i = 0; type && type[i] && o < zox_table_family_length - 2; i++) {
        const char c = type[i];
        if (c == '(') {
            in_pair = 1;
            after_comma = 0;
        } else if (c == ')') {
            in_pair = 0;
            if (after_comma) {
                out[o++] = '*';
            }
            after_comma = 0;
        } else if (c == ',' && in_pair) {
            out[o++] = c;
            after_comma = 1;
            continue;
        }
        if (!after_comma) {
            out[o++] = c;
        }
    }
    out[o] = 0;
    ecs_os_free(type);
}

// every table (empty ones too), sorted, returns how many, free it after
uintptr_t* zox_tables_snapshot(ecs* world, int* out_count) {
    int capacity = 4096, count = 0;
    uintptr_t* tables = malloc(capacity * sizeof(uintptr_t));
    ecs_query_t* query = ecs_query(world, {
        .terms = {{ .id = EcsAny }},
        .flags = EcsQueryMatchEmptyTables
    });
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        if (!it.table) {
            continue;
        }
        if (count >= capacity) {
            capacity *= 2;
            tables = realloc(tables, capacity * sizeof(uintptr_t));
        }
        tables[count++] = (uintptr_t) it.table;
    }
    ecs_query_fini(query);
    qsort(tables, count, sizeof(uintptr_t), zox_compare_pointers);
    *out_count = count;
    return tables;
}

// the families of tables made since a snapshot, most first, returns how many families
int zox_new_table_families(ecs* world, const uintptr_t* before, int before_count,
    char families[][zox_table_family_length], int* counts, int max, int* total)
{
    int count = 0;
    uintptr_t* now = zox_tables_snapshot(world, &count);
    int families_count = 0;
    *total = 0;
    static char family[zox_table_family_length];
    for (int i = 0; i < count; i++) {
        if (i > 0 && now[i] == now[i - 1]) {
            continue;
        }
        if (bsearch(&now[i], before, before_count, sizeof(uintptr_t), zox_compare_pointers)) {
            continue;
        }
        (*total)++;
        zox_table_family(world, (ecs_table_t*) now[i], family);
        int found = -1;
        for (int f = 0; f < families_count; f++) {
            if (!strcmp(families[f], family)) {
                found = f;
                break;
            }
        }
        if (found < 0 && families_count < max) {
            found = families_count++;
            snprintf(families[found], zox_table_family_length, "%s", family);
            counts[found] = 0;
        }
        if (found >= 0) {
            counts[found]++;
        }
    }
    free(now);
    for (int i = 1; i < families_count; i++) {
        for (int j = i; j > 0 && counts[j] > counts[j - 1]; j--) {
            static char swap[zox_table_family_length];
            memcpy(swap, families[j], zox_table_family_length);
            memcpy(families[j], families[j - 1], zox_table_family_length);
            memcpy(families[j - 1], swap, zox_table_family_length);
            const int c = counts[j];
            counts[j] = counts[j - 1];
            counts[j - 1] = c;
        }
    }
    return families_count;
}

int zox_table_families(ecs* world, char families[][zox_table_family_length], int* counts, int max, int* total) {
    // every table once, empty ones too (a table stays after its entities leave, made tables add up)
    int capacity = 4096, count = 0;
    uintptr_t* tables = malloc(capacity * sizeof(uintptr_t));
    ecs_query_t* query = ecs_query(world, {
        .terms = {{ .id = EcsAny }},
        .flags = EcsQueryMatchEmptyTables
    });
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        if (!it.table) {
            continue;
        }
        if (count >= capacity) {
            capacity *= 2;
            tables = realloc(tables, capacity * sizeof(uintptr_t));
        }
        tables[count++] = (uintptr_t) it.table;
    }
    ecs_query_fini(query);
    qsort(tables, count, sizeof(uintptr_t), zox_compare_pointers);
    int families_count = 0;
    *total = 0;
    static char family[zox_table_family_length];
    for (int i = 0; i < count; i++) {
        if (i > 0 && tables[i] == tables[i - 1]) {
            continue;
        }
        (*total)++;
        zox_table_family(world, (ecs_table_t*) tables[i], family);
        int found = -1;
        for (int f = 0; f < families_count; f++) {
            if (!strcmp(families[f], family)) {
                found = f;
                break;
            }
        }
        if (found < 0 && families_count < max) {
            found = families_count++;
            snprintf(families[found], zox_table_family_length, "%s", family);
            counts[found] = 0;
        }
        if (found >= 0) {
            counts[found]++;
        }
    }
    free(tables);
    // most tables first
    for (int i = 1; i < families_count; i++) {
        for (int j = i; j > 0 && counts[j] > counts[j - 1]; j--) {
            static char swap[zox_table_family_length];
            memcpy(swap, families[j], zox_table_family_length);
            memcpy(families[j], families[j - 1], zox_table_family_length);
            memcpy(families[j - 1], swap, zox_table_family_length);
            const int c = counts[j];
            counts[j] = counts[j - 1];
            counts[j - 1] = c;
        }
    }
    return families_count;
}

// Counts every add and remove per id while profiling (an observer on * and (*,*), only when profiling)
//  - the ids moved most in a frame are what make its merges and notifications slow
#define zox_id_counts_max 4096
typedef struct {
    ecs_id_t id;
    uint adds;
    uint removes;
} ZoxIdCount;
ZoxIdCount zox_id_counts[zox_id_counts_max];
ZoxIdCount zox_id_totals[zox_id_counts_max];    // the whole run
byte zox_id_counts_on = 0;

static inline void zox_count_id_in(ZoxIdCount* counts, ecs_iter_t* it) {
    const ecs_id_t id = it->event_id;
    uint slot = (uint) ((id * 2654435761u) ^ (id >> 32)) % zox_id_counts_max;
    for (int probe = 0; probe < 64; probe++) {
        ZoxIdCount* c = &counts[(slot + probe) % zox_id_counts_max];
        if (!c->id || c->id == id) {
            c->id = id;
            if (it->event == EcsOnAdd) {
                c->adds += it->count;
            } else {
                c->removes += it->count;
            }
            return;
        }
    }
}

static void zox_count_id_event(ecs_iter_t* it) {
    zox_count_id_in(zox_id_counts, it);
    zox_count_id_in(zox_id_totals, it);
}

void zox_count_ids_begin(ecs* world) {
    if (zox_id_counts_on) {
        return;
    }
    zox_id_counts_on = 1;
    ecs_observer(world, {
        .query.terms = {{ EcsWildcard }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = zox_count_id_event
    });
    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsWildcard, EcsWildcard) }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = zox_count_id_event
    });
}

// the ids moved most in counts, as text into out (it changes counts, reset them after)
static void zox_count_ids_text(ecs* world, ZoxIdCount* zox_id_counts, char* out, int size, int top) {
    int written = 0;
    out[0] = 0;
    for (int t = 0; t < top; t++) {
        int best = -1;
        uint best_total = 0;
        for (int i = 0; i < zox_id_counts_max; i++) {
            const uint total = zox_id_counts[i].adds + zox_id_counts[i].removes;
            if (zox_id_counts[i].id && total > best_total) {
                best_total = total;
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        char* name = ecs_id_str(world, zox_id_counts[best].id);
        // snprintf returns what it would write, past the end when cut, so stop when full
        const int length = snprintf(out + written, size - written, "%s%s +%u -%u", t ? ", " : "", name ? name : "?",
            zox_id_counts[best].adds, zox_id_counts[best].removes);
        ecs_os_free(name);
        zox_id_counts[best].adds = zox_id_counts[best].removes = 0;
        if (length < 0 || written + length >= size - 1) {
            break;
        }
        written += length;
    }
}

// the prefabs instantiated most since the last reset ((IsA, prefab) adds), what's spawning, as text
void zox_count_spawned_prefabs(ecs* world, char* out, int size, int top) {
    static ZoxIdCount isa[zox_id_counts_max];
    int count = 0;
    for (int i = 0; i < zox_id_counts_max; i++) {
        const ecs_id_t id = zox_id_counts[i].id;
        if (id && ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsIsA && zox_id_counts[i].adds) {
            isa[count++] = zox_id_counts[i];
        }
    }
    int written = 0;
    out[0] = 0;
    for (int t = 0; t < top; t++) {
        int best = -1;
        for (int i = 0; i < count; i++) {
            if (isa[i].adds && (best < 0 || isa[i].adds > isa[best].adds)) {
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        const char* name = ecs_get_name(world, ecs_pair_second(world, isa[best].id));
        const int length = snprintf(out + written, size - written, "%s%s x%u", t ? ", " : "", name ? name : "?", isa[best].adds);
        isa[best].adds = 0;
        if (length < 0 || written + length >= size - 1) {
            break;
        }
        written += length;
    }
}

// the ids moved most since the last reset (this frame), then resets them
void zox_count_ids_top(ecs* world, char* out, int size, int top) {
    zox_count_ids_text(world, zox_id_counts, out, size, top);
    memset(zox_id_counts, 0, sizeof(zox_id_counts));
}

// the ids moved most this run
void zox_count_ids_totals(ecs* world, char* out, int size, int top) {
    static ZoxIdCount copy[zox_id_counts_max];
    memcpy(copy, zox_id_totals, sizeof(copy));
    zox_count_ids_text(world, copy, out, size, top);
}

// flecs times each system (on in flecs_profiler builds, profiling tests turn it on)
void zox_measure_system_times(ecs* world) {
    ecs_measure_system_time(world, 1);
    ecs_measure_frame_time(world, 1);
    zox_time_observers = 1;
    zox_count_ids_begin(world);
}

// Spawns count entities in one go, straight into their table: ids (an IsA pair for a prefab, components,
//  tags) with data (an array of count values per component, NULL for tags and pairs)
//  - no moves between tables, one batch of notifications (spawning one at a time moves each entity
//      once per component set)
//  - not while deferred (inside multi threaded systems), run it from an immediate system or between frames
//  - returns the entities, flecs owns the array (copy it before spawning again)
const entity* zox_bulk_spawn(ecs* world, int count, const ecs_id_t* ids, void** data, int ids_count) {
    ecs_bulk_desc_t desc = { .count = count, .data = data };
    for (int i = 0; i < ids_count && i < FLECS_ID_DESC_MAX; i++) {
        desc.ids[i] = ids[i];
    }
    return ecs_bulk_init(world, &desc);
}

static inline ecs_id_t zox_isa_id(entity prefab) {
    return ecs_pair(EcsIsA, prefab);
}

static inline ecs_id_t zox_parent_id_non_fragment() {
    return ecs_id(EcsParent);
}

// non fragmenting parents for bulk spawned children (bulk data can't hold them: writing EcsParent as data
//  skipped the hook that adds a child to its parent's children, and modified didn't run it)
//  - spawn them with an empty EcsParent (zox_parent_id_non_fragment, zeroed data), setting it won't move them
void zox_bulk_parent(ecs* world, const entity* children, const entity* parents, int count) {
    for (int i = 0; i < count; i++) {
        ecs_set(world, children[i], EcsParent, { parents[i] });
    }
}
