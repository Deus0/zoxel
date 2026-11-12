// tst: npc_stats, every npc gets the realm's base stats (bulk spawned, character_stats_system)
//  - waits for npcs to spawn and initialize
//  - each initialized npc: every realm base stat exactly once (copied from its prefab, made at realm
//      generation, or spawned to fill a gap), parented
//      to it, energy and mana full
//  - run: bash bsh/tst.sh npc_stats
int zox_tst_npc_stats_frames = 0;

// npcs with stats, the most one frame has seen
static inline int zox_tst_npcs_with_stats(ecs* world) {
    int count = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Character));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            count += !zox_has(e, PlayerCharacter) && zox_valid(zox_get_child_by_id(world, e, zox_id(StatSoul)));
        }
    }
    return count;
}

// an npc that's finished initializing (its stats system has run)
static inline byte zox_tst_npc_ready(ecs* world, entity e) {
    return !zox_has(e, PlayerCharacter) && !zox_has(e, PreInitialize) && !zox_has(e, Initialize) &&
        zox_valid(zox_get_link(world, e, RealmLink));
}

byte zox_tst_npc_stats_ready(ecs* world) {
    zox_tst_npc_stats_frames++;
    zox_tst_waiting = "npcs initialized";
    int ready = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Character));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            ready += zox_tst_npc_ready(world, it.entities[i]);
        }
    }
    return ready >= 8;
}

void zox_tst_npc_stats_check(ecs* world) {
    entity realm = 0;
    {
        ecs_iter_t it0 = ecs_each_id(world, zox_id(Character));
        while (ecs_each_next(&it0)) {
            for (int i = 0; i < it0.count && !realm; i++) {
                if (zox_tst_npc_ready(world, it0.entities[i])) {
                    realm = zox_get_link(world, it0.entities[i], RealmLink);
                }
            }
        }
    }
    RealmBaseStats stats;
    zox_tst_check(zox_valid(realm) && get_realm_base_stats(world, realm, &stats), "no realm stats");
    if (!zox_valid(realm)) {
        return;
    }
    int npcs = 0, wrong = 0, wrong_parent = 0, not_full = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Character));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (!zox_tst_npc_ready(world, e)) {
                continue;
            }
            npcs++;
            // every realm base stat exactly once
            for (int s = 0; s < stats.count; s++) {
                int found = 0;
                iter it2 = zox_children(world, e);
                while (zox_children_next(it2)) {
                    for (int j = 0; j < it2.count; j++) {
                        const entity stat = it2.entities[j];
                        if (!is_realm_stat_of(world, stat, stats.all[s])) {
                            continue;
                        }
                        found++;
                        wrong_parent += zox_get_parent(world, stat) != e;
                        if (zox_has(stat, StatState) && stats.all[s] != stats.health) {
                            not_full += zox_getv(stat, StatValue) != zox_getv(stat, StatValueMax);
                        }
                    }
                }
                if (found != 1 && !wrong) {
                    zox_log("[tst] npc [%s] has [%i] of [%s]", zox_getn(e), found, zox_getn(stats.all[s]));
                }
                wrong += found != 1;
            }
        }
    }
    zox_log("[tst] [%i] npcs initialized, [%i] realm base stats", npcs, stats.count);
    zox_tst_check(npcs >= 8, "only [%i] npcs initialized", npcs);
    zox_tst_check(wrong == 0, "[%i] npc stats missing or doubled", wrong);
    zox_tst_check(wrong_parent == 0, "[%i] stats aren't their npc's children", wrong_parent);
    zox_tst_check(not_full == 0, "[%i] npc energy or mana don't start full", not_full);
}
