// tst: mass_delete, deleting many npc shaped entity trees at once, like far chunks' npcs going while flying
//  - 200 npcs, each with 8 child parts, each with 4 grandchild parts (41 entities an npc, modular like npcs)
//  - built twice: ChildOf parents (zox_set_parent), and non fragmenting parents (zox_set_parent_non_fragment)
//  - deletes the npcs (their trees go by cascade) deferred, like a merge, and times it
//  - logs each's time, time per npc, commands and observer notifications, and how they compare
//  - checks every npc and part is gone
//  - uses mass_spawn's components (TstSpawnPosition, TstSpawnHealth, TstSpawnSeed, TstSpawnValue)
//  - run: bash bsh/tst.sh mass_delete
#define zox_tst_delete_npcs 200
#define zox_tst_delete_children 8
#define zox_tst_delete_grandchildren 4
#define zox_tst_delete_parts (zox_tst_delete_children + zox_tst_delete_children * zox_tst_delete_grandchildren)

entity zox_tst_delete_npc_list[zox_tst_delete_npcs];
entity zox_tst_delete_part_list[zox_tst_delete_npcs * zox_tst_delete_parts];

byte zox_tst_mass_delete_ready(ecs* world) {
    (void) world;
    return 1;
}

static void zox_tst_delete_build(ecs* world, entity prefab, entity prefab_part, byte non_fragment) {
    int p = 0;
    zox_defer_begin();
    for (int i = 0; i < zox_tst_delete_npcs; i++) {
        const entity npc = zox_ins(world, prefab);
        zox_set(npc, TstSpawnSeed, { i });
        zox_tst_delete_npc_list[i] = npc;
        for (int c = 0; c < zox_tst_delete_children; c++) {
            const entity child = zox_ins(world, prefab_part);
            if (non_fragment) {
                zox_set_parent_non_fragment(world, child, npc);
            } else {
                zox_set_parent(world, child, npc);
            }
            zox_set(child, TstSpawnValue, { c });
            zox_tst_delete_part_list[p++] = child;
            for (int g = 0; g < zox_tst_delete_grandchildren; g++) {
                const entity grandchild = zox_ins(world, prefab_part);
                if (non_fragment) {
                    zox_set_parent_non_fragment(world, grandchild, child);
                } else {
                    zox_set_parent(world, grandchild, child);
                }
                zox_set(grandchild, TstSpawnValue, { g });
                zox_tst_delete_part_list[p++] = grandchild;
            }
        }
    }
    zox_defer_end();
}

static double zox_tst_delete_measure(ecs* world, const char* name) {
    const ZoxWorldTimes before = zox_world_times(world);
    const double start = zox_observer_now_ms();
    zox_defer_begin();
    for (int i = 0; i < zox_tst_delete_npcs; i++) {
        zox_delete(zox_tst_delete_npc_list[i]);
    }
    zox_defer_end();
    const double ms = zox_observer_now_ms() - start;
    const ZoxWorldTimes after = zox_world_times(world);
    zox_log("[tst] [metric] mass_delete %s [%i] npcs of [%i] entities: [%.1fms], [%.3fms] an npc, deletes [%li] removes [%li], observers ran [%li]",
        name, zox_tst_delete_npcs, 1 + zox_tst_delete_parts, ms, ms / zox_tst_delete_npcs,
        (long) (after.deletes - before.deletes), (long) (after.removes - before.removes),
        (long) (after.observers - before.observers));
    int alive = 0;
    for (int i = 0; i < zox_tst_delete_npcs; i++) {
        alive += zox_alive(zox_tst_delete_npc_list[i]);
    }
    for (int i = 0; i < zox_tst_delete_npcs * zox_tst_delete_parts; i++) {
        alive += zox_alive(zox_tst_delete_part_list[i]);
    }
    zox_tst_check(alive == 0, "%s: [%i] npcs or parts still alive after deleting", name, alive);
    return ms;
}

void zox_tst_mass_delete_check(ecs* world) {
    zox_measure_system_times(world);
    const entity prefab = zox_new();
    zox_make_prefab(prefab);
    zox_set(prefab, TstSpawnPosition, { float3_zero });
    zox_set(prefab, TstSpawnHealth, { 100 });
    zox_set(prefab, TstSpawnSeed, { 0 });
    const entity prefab_part = zox_new();
    zox_make_prefab(prefab_part);
    zox_set(prefab_part, TstSpawnValue, { 0 });
    zox_tst_delete_build(world, prefab, prefab_part, 0);
    const double childof_ms = zox_tst_delete_measure(world, "childof");
    zox_tst_delete_build(world, prefab, prefab_part, 1);
    const double non_fragment_ms = zox_tst_delete_measure(world, "non fragmenting");
    zox_log("[tst] [metric] mass_delete non fragmenting is [%.1f]x the time of childof",
        childof_ms > 0 ? non_fragment_ms / childof_ms : 0);
    zox_delete(prefab);
    zox_delete(prefab_part);
}
