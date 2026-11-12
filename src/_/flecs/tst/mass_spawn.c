// tst: mass_spawn, spawning many npcs at once: one at a time (as spawn systems do) vs in bulk
//  - 2000 npc like entities from a prefab (position, health, seed), each with 3 non fragmenting stat
//      children (from a stat prefab, a value each)
//  - single: instance, set each component, spawn and parent each child, deferred like a system
//  - bulk: the npcs in one zox_bulk_spawn with their data, their stats in another with an empty parent,
//      then their parents set (zox_bulk_parent, EcsParent as bulk data skipped registering the children,
//      the empty one in their table means setting it doesn't move them)
//  - found (debug): single 77ms, bulk 36ms, the npcs alone 3ms in bulk (24x), parenting is the rest
//  - logs each's time, commands, observer notifications and tables made
//  - checks both made every entity with its values, the stats' parents and the npcs' children
//  - run: bash bsh/tst.sh mass_spawn
#define zox_tst_spawn_count 2000
#define zox_tst_spawn_stats 3
zoxc_float3(TstSpawnPosition);
zoxc_float(TstSpawnHealth);
zoxc_float(TstSpawnValue);
zoxc_int(TstSpawnSeed);

void zox_components_tst_mass_spawn(ecs* world) {
    zoxd_float3(TstSpawnPosition);
    zoxd_float(TstSpawnHealth);
    zoxd_float(TstSpawnValue);
    zoxd_int(TstSpawnSeed);
}

byte zox_tst_mass_spawn_ready(ecs* world) {
    (void) world;
    return 1;
}

typedef struct {
    double ms;
    ZoxWorldTimes before;
} ZoxTstSpawnMeasure;

static inline ZoxTstSpawnMeasure zox_tst_spawn_begin(ecs* world) {
    return (ZoxTstSpawnMeasure) { zox_observer_now_ms(), zox_world_times(world) };
}

static inline void zox_tst_spawn_end(ecs* world, ZoxTstSpawnMeasure m, const char* name, double* out_ms) {
    const double ms = zox_observer_now_ms() - m.ms;
    const ZoxWorldTimes after = zox_world_times(world);
    zox_log("[tst] [metric] mass_spawn %s [%i] npcs with [%i] stats each: [%.1fms], adds [%li] sets [%li], observers ran [%li], tables made [%li]",
        name, zox_tst_spawn_count, zox_tst_spawn_stats, ms, (long) (after.adds - m.before.adds), (long) (after.sets - m.before.sets),
        (long) (after.observers - m.before.observers), (long) (after.tables - m.before.tables));
    *out_ms = ms;
}

// both ways made every npc with its values, and its stats as children
static inline void zox_tst_spawn_verify(ecs* world, const entity* npcs, entity stats[][zox_tst_spawn_stats], const char* name) {
    int wrong = 0, orphans = 0, missing_children = 0;
    for (int i = 0; i < zox_tst_spawn_count; i++) {
        const entity e = npcs[i];
        if (!zox_alive(e) || zox_getv(e, TstSpawnSeed) != i || zox_getv(e, TstSpawnHealth) != 100 + i) {
            wrong++;
            continue;
        }
        for (int s = 0; s < zox_tst_spawn_stats; s++) {
            orphans += zox_get_parent(world, stats[i][s]) != e || zox_getv(stats[i][s], TstSpawnValue) != s + 1;
        }
        missing_children += zox_get_children_count(world, e) != zox_tst_spawn_stats;
    }
    zox_tst_check(wrong == 0, "%s: [%i] npcs missing or with wrong values", name, wrong);
    zox_tst_check(orphans == 0, "%s: [%i] stats with the wrong parent or value", name, orphans);
    zox_tst_check(missing_children == 0, "%s: [%i] npcs without their [%i] stats as children", name, missing_children, zox_tst_spawn_stats);
}

void zox_tst_mass_spawn_check(ecs* world) {
    zox_measure_system_times(world);
    const entity prefab = zox_new();
    zox_make_prefab(prefab);
    zox_prefab_set(prefab, TstSpawnPosition, { float3_zero });
    zox_prefab_set(prefab, TstSpawnHealth, { 0 });
    zox_prefab_set(prefab, TstSpawnSeed, { 0 });
    const entity prefab_stat = zox_new();
    zox_make_prefab(prefab_stat);
    zox_prefab_set(prefab_stat, TstSpawnValue, { 0 });
    static entity single[zox_tst_spawn_count];
    static entity single_stats[zox_tst_spawn_count][zox_tst_spawn_stats];
    static entity bulk[zox_tst_spawn_count];
    static entity bulk_stats[zox_tst_spawn_count][zox_tst_spawn_stats];
    double single_ms = 0, bulk_ms = 0;
    // one at a time, deferred like a spawn system
    {
        ZoxTstSpawnMeasure m = zox_tst_spawn_begin(world);
        zox_defer_begin();
        for (int i = 0; i < zox_tst_spawn_count; i++) {
            const entity e = zox_ins(world, prefab);
            zox_set(e, TstSpawnPosition, { (float3) { i, 0, 0 } });
            zox_set(e, TstSpawnHealth, { 100 + i });
            zox_set(e, TstSpawnSeed, { i });
            single[i] = e;
            for (int s = 0; s < zox_tst_spawn_stats; s++) {
                const entity stat = zox_ins(world, prefab_stat);
                zox_set_parent_non_fragment(world, stat, e);
                zox_set(stat, TstSpawnValue, { s + 1 });
                single_stats[i][s] = stat;
            }
        }
        zox_defer_end();
        zox_tst_spawn_end(world, m, "single", &single_ms);
    }
    // in bulk: the npcs, then all their stats
    {
        static TstSpawnPosition positions[zox_tst_spawn_count];
        static TstSpawnHealth healths[zox_tst_spawn_count];
        static TstSpawnSeed seeds[zox_tst_spawn_count];
        static EcsParent parents[zox_tst_spawn_count * zox_tst_spawn_stats];
        static TstSpawnValue values[zox_tst_spawn_count * zox_tst_spawn_stats];
        ZoxTstSpawnMeasure m = zox_tst_spawn_begin(world);
        for (int i = 0; i < zox_tst_spawn_count; i++) {
            positions[i].value = (float3) { i, 0, 0 };
            healths[i].value = 100 + i;
            seeds[i].value = i;
        }
        const ecs_id_t npc_ids[] = { zox_isa_id(prefab), zox_id(TstSpawnPosition), zox_id(TstSpawnHealth), zox_id(TstSpawnSeed) };
        void* npc_data[] = { NULL, positions, healths, seeds };
        const entity* spawned = zox_bulk_spawn(world, zox_tst_spawn_count, npc_ids, npc_data, 4);
        memcpy(bulk, spawned, sizeof(bulk));
        for (int i = 0; i < zox_tst_spawn_count; i++) {
            for (int s = 0; s < zox_tst_spawn_stats; s++) {
                parents[i * zox_tst_spawn_stats + s].value = bulk[i];
                values[i * zox_tst_spawn_stats + s].value = s + 1;
            }
        }
        // an empty parent in their table already, so setting it doesn't move them
        static EcsParent no_parents[zox_tst_spawn_count * zox_tst_spawn_stats];
        const ecs_id_t stat_ids[] = { zox_isa_id(prefab_stat), zox_id(TstSpawnValue), zox_parent_id_non_fragment() };
        void* stat_data[] = { NULL, values, no_parents };
        const entity* spawned_stats = zox_bulk_spawn(world, zox_tst_spawn_count * zox_tst_spawn_stats, stat_ids, stat_data, 3);
        static entity stats_flat[zox_tst_spawn_count * zox_tst_spawn_stats];
        static entity parents_flat[zox_tst_spawn_count * zox_tst_spawn_stats];
        memcpy(stats_flat, spawned_stats, sizeof(stats_flat));
        for (int i = 0; i < zox_tst_spawn_count * zox_tst_spawn_stats; i++) {
            parents_flat[i] = parents[i].value;
        }
        zox_bulk_parent(world, stats_flat, parents_flat, zox_tst_spawn_count * zox_tst_spawn_stats);
        for (int i = 0; i < zox_tst_spawn_count; i++) {
            for (int s = 0; s < zox_tst_spawn_stats; s++) {
                bulk_stats[i][s] = stats_flat[i * zox_tst_spawn_stats + s];
            }
        }
        zox_tst_spawn_end(world, m, "bulk", &bulk_ms);
    }
    zox_tst_spawn_verify(world, single, single_stats, "single");
    zox_tst_spawn_verify(world, bulk, bulk_stats, "bulk");
    zox_log("[tst] [metric] mass_spawn bulk is [%.1f]x faster", bulk_ms > 0 ? single_ms / bulk_ms : 0);
    for (int i = 0; i < zox_tst_spawn_count; i++) {
        zox_delete(single[i]);
        zox_delete(bulk[i]);
    }
    zox_delete(prefab);
    zox_delete(prefab_stat);
}
