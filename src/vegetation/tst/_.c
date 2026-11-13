// tst: vegetation_trees, trees are spaced apart in a jittered grid (vegetation/fun)
//  - over a big area: trees at least zox_tree_cell - 2 * (cell - margin) apart (5 blocks), never touching
//  - each tree's canopy stays inside its cell (so a chunk draws whole trees), trees a cell near the biome's chance
//  - the same seed, the same trees
//  - run: bash bsh/tst.sh vegetation_trees
// tst: vegetation_cover, ground cover follows a biome's shares, grasslands are grassy (lots of tall grass)
//  - run: bash bsh/tst.sh vegetation_cover
#define zox_tst_veg_area 512
#define zox_tst_veg_seed 31337

byte zox_tst_vegetation_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_vegetation_trees_check(ecs* world) {
    (void) world;
    const float chance = 0.45f;
    static int2 trees[zox_tst_veg_area * zox_tst_veg_area / 16];
    int count = 0, differs = 0, canopy_out = 0;
    for (int x = 0; x < zox_tst_veg_area; x++) {
        for (int y = 0; y < zox_tst_veg_area; y++) {
            const int2 p = (int2) { x, y };
            const byte tree = is_tree_at(p, zox_tst_veg_seed, chance);
            differs += tree != is_tree_at(p, zox_tst_veg_seed, chance);
            if (!tree) {
                continue;
            }
            if (count < (int) (sizeof(trees) / sizeof(int2))) {
                trees[count++] = p;
            }
            // its canopy in its cell
            const int cx = vegetation_floor_div(x, zox_tree_cell) * zox_tree_cell;
            const int cy = vegetation_floor_div(y, zox_tree_cell) * zox_tree_cell;
            canopy_out += x - zox_tree_canopy < cx || x + zox_tree_canopy >= cx + zox_tree_cell ||
                y - zox_tree_canopy < cy || y + zox_tree_canopy >= cy + zox_tree_cell;
        }
    }
    int closest = 1 << 30;
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            const int dx = abs(trees[i].x - trees[j].x), dy = abs(trees[i].y - trees[j].y);
            if (dx > 16 || dy > 16) {
                continue;
            }
            const int d = dx > dy ? dx : dy;
            closest = d < closest ? d : closest;
        }
    }
    const int cells = (zox_tst_veg_area / zox_tree_cell) * (zox_tst_veg_area / zox_tree_cell);
    const float per_cell = (float) count / cells;
    zox_log("[tst] [metric] vegetation_trees [%i] trees in [%i] cells (%.2f a cell, chance %.2f), closest [%i] blocks",
        count, cells, per_cell, chance, closest);
    zox_tst_check(per_cell > chance - 0.05f && per_cell < chance + 0.05f, "[%.2f] trees a cell, the chance is [%.2f]", per_cell, chance);
    zox_tst_check(closest >= 2 * zox_tree_canopy + 1, "trees [%i] blocks apart, their canopies touch", closest);
    zox_tst_check(!canopy_out, "[%i] trees' canopies leave their cell", canopy_out);
    zox_tst_check(!differs, "[%i] columns changed with the same seed", differs);
}

void zox_tst_vegetation_cover_check(ecs* world) {
    (void) world;
    // grasslands, desert, jungle (gam/zoxel/sys/biomes.c)
    const float biomes[3][4] = { { 0.02f, 0.28f, 0.55f, 0.10f }, { 0.70f, 0.20f, 0.06f, 0.01f }, { 0.02f, 0.18f, 0.62f, 0.14f } };
    const char* names[3] = { "grasslands", "desert", "jungle" };
    for (byte b = 0; b < 3; b++) {
        int counts[5] = { 0 };
        int total = 0;
        for (int x = 0; x < zox_tst_veg_area; x++) {
            for (int y = 0; y < zox_tst_veg_area; y++) {
                counts[get_ground_cover((int2) { x * 3, y * 3 }, 16, zox_tst_veg_seed + b, biomes[b])]++;
                total++;
            }
        }
        const float dirt = (float) counts[zox_vegetation_dirt] / total;
        const float weeds = (float) counts[zox_vegetation_weeds] / total;
        const float flowers = (float) counts[zox_vegetation_flowers] / total;
        zox_log("[tst] [metric] vegetation_cover %s: dirt [%.2f] tall grass [%.2f] flowers [%.2f] (shares %.2f %.2f %.2f)",
            names[b], dirt, weeds, flowers, biomes[b][0], biomes[b][2], biomes[b][3]);
        // patches move columns between neighbours, the shares hold within a margin
        zox_tst_check(fabsf(dirt - biomes[b][0]) < 0.08f, "%s dirt [%.2f], share [%.2f]", names[b], dirt, biomes[b][0]);
        zox_tst_check(fabsf(weeds - biomes[b][2]) < 0.08f, "%s tall grass [%.2f], share [%.2f]", names[b], weeds, biomes[b][2]);
    }
}

void zox_tsts_vegetation() {
    zox_tst_add("vegetation_trees", zox_tst_vegetation_ready, zox_tst_vegetation_trees_check, 30);
    zox_tst_add("vegetation_cover", zox_tst_vegetation_ready, zox_tst_vegetation_cover_check, 30);
}
