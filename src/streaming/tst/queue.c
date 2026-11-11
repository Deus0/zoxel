// tst: spawn_queue, the terrain spawn queue (TerrainSpawnQueue) keeps its positions hash in sync
//  - a cleared queue accepts the same positions again
//    (the town placer cleared only the count, positions left in the hash could never be queued again,
//     so the player's terrain column never spawned, ~1 in 3 new games)
//  - many add / remove rounds end (removed slots are tombstones, a table full of them never ended a lookup)
//  - closest first, and every queued position comes out once
//  - run: bash bsh/tst.sh spawn_queue

byte zox_tst_spawn_queue_ready(ecs* world) {
    (void) world;
    return 1;
}

static inline TerrainSpawnUpdate zox_tst_spawn_update(int x, int z, int distance) {
    return (TerrainSpawnUpdate) {
        .position = (int2) { x, z },
        .distance = distance,
        .level = 1,
    };
}

void zox_tst_spawn_queue_check(ecs* world) {
    (void) world;
    TerrainSpawnQueue q = { 0 };
    i_TerrainSpawnQueue(&q);
    const int side = 15;
    // fill, clear, fill again: every position is accepted both times
    for (int pass = 0; pass < 2; pass++) {
        int added = 0;
        for (int x = 0; x < side; x++) for (int z = 0; z < side; z++) {
            added += add_or_set_TerrainSpawnQueue(&q, zox_tst_spawn_update(x, z, x + z));
        }
        zox_tst_check(added == side * side, "pass [%i] queued [%i] of [%i] positions", pass, added, side * side);
        // half processed, like the town placer clears mid stream
        for (int i = 0; i < side * side / 2; i++) {
            remove_closest_TerrainSpawnQueue(&q);
        }
        clear_TerrainSpawnQueue(&q);
        zox_tst_check(q.count == 0 && q.hash_count == 0, "clear left count [%i] hash [%i]", (int) q.count, (int) q.hash_count);
    }
    // closest first, each position once
    for (int x = 0; x < side; x++) for (int z = 0; z < side; z++) {
        add_or_set_TerrainSpawnQueue(&q, zox_tst_spawn_update(x, z, x + z));
    }
    int last_distance = -1;
    int out = 0;
    byte ordered = 1;
    while (q.count) {
        TerrainSpawnUpdate u = remove_closest_TerrainSpawnQueue(&q);
        ordered &= u.distance >= last_distance;
        last_distance = u.distance;
        out++;
    }
    zox_tst_check(ordered, "positions didnt come out closest first");
    zox_tst_check(out == side * side, "[%i] of [%i] positions came out", out, side * side);
    // churn: lots of add / remove, the hash must keep empty slots so lookups end
    int accepted = 0;
    for (int round = 0; round < 2000; round++) {
        for (int k = 0; k < 8; k++) {
            accepted += add_TerrainSpawnQueue(&q, zox_tst_spawn_update(round * 8 + k, round, k));
        }
        while (q.count) {
            remove_closest_TerrainSpawnQueue(&q);
        }
    }
    zox_tst_check(accepted == 2000 * 8, "churn queued [%i] of [%i]", accepted, 2000 * 8);
    zox_tst_check(q.hash_count + q.hash_tombstones < q.hash_capacity,
        "hash has no empty slots, used [%i] tombstones [%i] capacity [%i]",
        (int) q.hash_count, (int) q.hash_tombstones, (int) q.hash_capacity);
    zox_log("[tst] spawn queue hash capacity [%i] after churn", (int) q.hash_capacity);
    d_TerrainSpawnQueue(&q);
}

void zox_tsts_streaming() {
    zox_tst_add("spawn_queue", zox_tst_spawn_queue_ready, zox_tst_spawn_queue_check, 30);
}
