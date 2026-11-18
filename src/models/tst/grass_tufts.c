// tst: grass_tufts, tall grass is tufts of stalks, not a mass of them (build_vox_tufts, models/generation/rubble.c)
//  - a depth 4 block, 16 x 16 columns, given 40 stalks (grass.c gives 12 to 22 percent of its columns)
//  - 40 columns have a stalk, the rest are empty; their heights vary; the middle's are taller than the edges' on
//    average; another seed puts them elsewhere
//  - run: bash bsh/tst.sh grass_tufts
byte zox_tst_grass_tufts_ready(ecs* world) {
    (void) world;
    return 1;
}

// a block's tufts: stalk columns, their mean height in the middle 8 x 8 and around it, the tallest and shortest
static inline int zox_tst_tufts_measure(uint seed, float* middle, float* edge, int* tallest, int* shortest, uint* hash) {
    const byte depth = 4, length = 16, max_height = 15;
    VoxelNode voxels;
    octree_create(&voxels, sizeof(VoxelNode), 0);
    build_vox_tufts(&voxels, depth, (byte2) { 1, 3 }, max_height, 40, seed);
    int stalks = 0, middle_count = 0, edge_count = 0, middle_sum = 0, edge_sum = 0;
    *tallest = 0;
    *shortest = 255;
    *hash = 0;
    for (byte x = 0; x < length; x++) for (byte z = 0; z < length; z++) {
        int height = 0;
        while (height < length && getv_VoxelNode(&voxels, depth, (byte3) { x, height, z })) {
            height++;
        }
        if (!height) {
            continue;
        }
        stalks++;
        *hash = *hash * 31 + x * 16 + z;
        *tallest = int_max(*tallest, height);
        *shortest = int_min(*shortest, height);
        const byte in_middle = x >= 4 && x < 12 && z >= 4 && z < 12;
        middle_sum += in_middle ? height : 0;
        middle_count += in_middle;
        edge_sum += in_middle ? 0 : height;
        edge_count += !in_middle;
    }
    close_VoxelNode(&voxels);
    *middle = middle_count ? (float) middle_sum / middle_count : 0;
    *edge = edge_count ? (float) edge_sum / edge_count : 0;
    return stalks;
}

void zox_tst_grass_tufts_check(ecs* world) {
    (void) world;
    float middle, edge, middle2, edge2;
    int tallest, shortest, tallest2, shortest2;
    uint hash, hash2;
    const int stalks = zox_tst_tufts_measure(123, &middle, &edge, &tallest, &shortest, &hash);
    zox_tst_tufts_measure(456, &middle2, &edge2, &tallest2, &shortest2, &hash2);
    zox_log("[tst] [metric] grass_tufts [%i] stalks of 256 columns, heights [%i] to [%i], middle [%.1f] edge [%.1f]",
        stalks, shortest, tallest, middle, edge);
    zox_tst_check(stalks == 40, "[%i] columns have a stalk, not 40", stalks);
    zox_tst_check(tallest - shortest >= 5, "stalk heights barely vary: [%i] to [%i]", shortest, tallest);
    zox_tst_check(middle > edge, "the middle's stalks [%.1f] aren't taller than the edges' [%.1f]", middle, edge);
    zox_tst_check(hash != hash2, "another seed put the stalks in the same columns");
}
