// tst: hashmaps, the maps chunks, tunks, regions and textures are looked up in
//  - int3 (chunks): random add / get / has / remove against a reference list, and count
//  - a streamed terrain's chunks spread over the buckets (short chains)
//  - adding an existing key replaces it (a duplicate made remove leave the old value behind)
//  - only hashes are stored, so keys in a streaming window must never share a hash:
//      int3 over a 129 x 15 x 129 window, int2 over 513 x 513, and generated texture names
//  - run: bash bsh/tst.sh hashmaps
//  - included from src/core/tst/_.c, as "_" is below the tst harness

static int zox_tst_compare_uint(const void* a, const void* b) {
    const uint x = *(const uint*) a;
    const uint y = *(const uint*) b;
    return x < y ? -1 : x > y;
}

// sorts hashes and counts repeats
static inline int zox_tst_count_duplicates(uint* hashes, int count) {
    qsort(hashes, count, sizeof(uint), zox_tst_compare_uint);
    int duplicates = 0;
    for (int i = 1; i < count; i++) {
        duplicates += hashes[i] == hashes[i - 1];
    }
    return duplicates;
}

byte zox_tst_hashmaps_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_hashmaps_check(ecs* world) {
    (void) world;
    // random operations against a reference list
    #define zox_tst_hashmap_keys 3000
    static int3 keys[zox_tst_hashmap_keys];
    static entity values[zox_tst_hashmap_keys];    // 0 = not in the map
    uint seed = 99;
    for (int i = 0; i < zox_tst_hashmap_keys; i++) {
        // unique keys in a streaming sized window
        keys[i] = (int3) { (i % 40) - 20, ((i / 40) % 9) - 4, (i / 360) - 4 };
        values[i] = 0;
    }
    int3_hashmap* map = create_int3_hashmap(256);
    int wrong = 0;
    for (int step = 0; step < 40000; step++) {
        seed = seed * 1103515245 + 12345;
        const int k = (seed >> 8) % zox_tst_hashmap_keys;
        const int op = (seed >> 24) % 3;
        if (op == 0) {
            values[k] = 1000 + step;
            int3_hashmap_add(map, keys[k], values[k]);
        } else if (op == 1) {
            int3_hashmap_remove(map, keys[k]);
            values[k] = 0;
        } else {
            const entity got = int3_hashmap_get(map, keys[k]);
            const byte has = int3_hashmap_has(map, keys[k]);
            if (got != values[k] || has != (values[k] != 0)) {
                if (!wrong++) {
                    zox_loge("[tst] key [%ix%ix%i] got [%lu] has [%i], expected [%lu]",
                        keys[k].x, keys[k].y, keys[k].z, (unsigned long) got, has, (unsigned long) values[k]);
                }
            }
        }
    }
    int expected_count = 0;
    for (int i = 0; i < zox_tst_hashmap_keys; i++) {
        expected_count += values[i] != 0;
        if (int3_hashmap_get(map, keys[i]) != values[i]) {
            wrong++;
        }
    }
    zox_tst_check(!wrong, "int3 hashmap was wrong [%i] times against the reference", wrong);
    zox_tst_check(count_int3_hashmap(map) == expected_count, "int3 hashmap counts [%i], expected [%i]", count_int3_hashmap(map), expected_count);
    // add twice then remove, the key is gone (no stale duplicate)
    const int3 twice = { 123, 2, -77 };
    int3_hashmap_add(map, twice, 1);
    int3_hashmap_add(map, twice, 2);
    zox_tst_check(int3_hashmap_get(map, twice) == 2, "adding a key twice didn't replace its value");
    int3_hashmap_remove(map, twice);
    zox_tst_check(!int3_hashmap_has(map, twice) && !int3_hashmap_get(map, twice), "removed key still found (stale duplicate)");
    int3_hashmap_dispose(map);
    // a streamed terrain's worth of chunks spreads over the buckets (terrain uses 32 * 32)
    map = create_int3_hashmap(32 * 32);
    for (int x = -16; x <= 16; x++) for (int y = -3; y <= 3; y++) for (int z = -4; z <= 4; z++) {
        int3_hashmap_add(map, (int3) { x + 100, y, z - 250 }, 1);
    }
    int longest = 0;
    for (uint b = 0; b < map->size; b++) {
        int length = 0;
        for (int3_hashmap_pair* pair = map->data[b]; pair; pair = pair->next) {
            length++;
        }
        if (length > longest) {
            longest = length;
        }
    }
    zox_log("[tst] [%i] chunks in [%u] buckets, longest chain [%i]", count_int3_hashmap(map), map->size, longest);
    zox_tst_check(longest <= 12, "longest hashmap chain is [%i] for [%i] chunks in [%u] buckets", longest, count_int3_hashmap(map), map->size);
    int3_hashmap_dispose(map);
    // no two chunk positions in a window share a hash
    const int wx = 129, wy = 15, wz = 129;
    uint* hashes = malloc(sizeof(uint) * wx * wy * wz);
    int count = 0;
    for (int x = 0; x < wx; x++) for (int y = 0; y < wy; y++) for (int z = 0; z < wz; z++) {
        hashes[count++] = get_int3_hash((int3) { x - wx / 2 + 5000, y - wy / 2, z - wz / 2 - 3000 });
    }
    int duplicates = zox_tst_count_duplicates(hashes, count);
    zox_tst_check(!duplicates, "[%i] int3 hash collisions in a [%ix%ix%i] window", duplicates, wx, wy, wz);
    free(hashes);
    // int2 (tunks, regions)
    const int w2 = 513;
    hashes = malloc(sizeof(uint) * w2 * w2);
    count = 0;
    for (int x = 0; x < w2; x++) for (int y = 0; y < w2; y++) {
        hashes[count++] = get_int2_hash((int2) { x - w2 / 2 - 7000, y - w2 / 2 + 1234 });
    }
    duplicates = zox_tst_count_duplicates(hashes, count);
    zox_tst_check(!duplicates, "[%i] int2 hash collisions in a [%ix%i] window", duplicates, w2, w2);
    free(hashes);
    // texture style names (string hashmap keys)
    const char* prefixes[] = { "block_", "item_", "icon_", "taskbar_", "skill_", "stat_", "quest_", "ui_" };
    const char* suffixes[] = { "", "_dark", "_light", "_icon", "_frame", "_2" };
    hashes = malloc(sizeof(uint) * 8 * 6 * 200);
    count = 0;
    for (int p = 0; p < 8; p++) for (int s = 0; s < 6; s++) for (int n = 0; n < 200; n++) {
        char name[64];
        snprintf(name, sizeof(name), "%s%i%s", prefixes[p], n, suffixes[s]);
        hashes[count++] = get_string_hash((string_data) { name, strlen(name) });
    }
    duplicates = zox_tst_count_duplicates(hashes, count);
    zox_tst_check(!duplicates, "[%i] string hash collisions in [%i] texture names", duplicates, count);
    free(hashes);
}

void zox_tsts_hashmaps() {
    zox_tst_add("hashmaps", zox_tst_hashmaps_ready, zox_tst_hashmaps_check, 30);
}
