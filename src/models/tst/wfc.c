// tst: wfc, wave function collapse (fun/wfc.c) on a small made up source: a floor slab with pillars, red tops
//  - only the source's voxels; every window of an output (air outside it) is one of the source's patches (3 and 2)
//  - the same seed the same output, other seeds other outputs
//  - rules: floor (row 0 solid somewhere), mirror (x mirrored), one piece (one piece, at least half the source)
//  - pure logic on the main menu; run: bash bsh/tst.sh wfc
#define zox_tst_wfc_source_size (int3) { 9, 6, 9 }
#define zox_tst_wfc_size (int3) { 14, 7, 12 }

byte zox_tst_wfc_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_wfc_source(byte* source) {
    const int3 s = zox_tst_wfc_source_size;
    memset(source, 0, s.x * s.y * s.z);
    for (int x = 0; x < s.x; x++)
    for (int z = 0; z < s.z; z++) {
        source[x + z * s.x * s.y] = 1;
    }
    const int3 pillars[5] = { { 1, 3, 1 }, { 4, 4, 4 }, { 7, 2, 1 }, { 1, 5, 7 }, { 7, 3, 7 } };  // x, height, z
    for (int i = 0; i < 5; i++) {
        for (int y = 1; y <= pillars[i].y; y++) {
            source[pillars[i].x + y * s.x + pillars[i].z * s.x * s.y] = y == pillars[i].y ? 2 : 1;
        }
    }
}

int zox_tst_wfc_solid(const byte* grid, int3 size) {
    int n = 0;
    for (int i = 0; i < size.x * size.y * size.z; i++) {
        n += grid[i] != 0;
    }
    return n;
}

// windows of out on its cells not among the source's patches
int zox_tst_wfc_strangers(const wfc_patches* p, const byte* out, int3 size) {
    int strangers = 0;
    byte patch[27];
    for (int z = 0; z < size.z; z++)
    for (int y = 0; y < size.y; y++)
    for (int x = 0; x < size.x; x++) {
        for (int i = 0; i < p->volume; i++) {
            patch[i] = wfc_get(out, size, x + wfc_axis(p, i, 0) + p->offset.x, y + wfc_axis(p, i, 1) + p->offset.y,
                z + wfc_axis(p, i, 2) + p->offset.z);
        }
        byte found = 0;
        for (int k = 0; k < p->count && !found; k++) {
            found = !memcmp(p->voxels + k * p->volume, patch, p->volume);
        }
        strangers += !found;
    }
    return strangers;
}

void zox_tst_wfc_check(ecs* world) {
    (void) world;
    const int3 s = zox_tst_wfc_source_size;
    const int3 size = zox_tst_wfc_size;
    const int cells = size.x * size.y * size.z;
    byte source[9 * 6 * 9];
    zox_tst_wfc_source(source);
    const int source_solid = zox_tst_wfc_solid(source, s);
    byte* a = malloc(cells);
    byte* b = malloc(cells);
    // plain (air outside): every window a source patch, only its voxels
    const byte rules = wfc_rule_air_border;
    const int3 sides[3] = { { 2, 2, 2 }, { 3, 3, 3 }, wfc_patch_size };
    for (int j = 0; j < 3; j++) {
        const int3 n = sides[j];
        wfc_patches patches = wfc_patches_make(source, s, n);
        zox_log("[tst] wfc: [%i] patches of [%ix%ix%i] from [%i] voxels", patches.count, n.x, n.y, n.z, source_solid);
        zox_tst_check(wfc_generate(source, s, a, size, 7, rules, n), "no output for seed 7, patches of [%ix%ix%i]", n.x, n.y, n.z);
        int foreign = 0;
        for (int i = 0; i < cells; i++) {
            foreign += a[i] > 2;
        }
        zox_tst_check(!foreign, "[%i] voxels not in the source", foreign);
        zox_tst_check(zox_tst_wfc_solid(a, size) > 0, "the output is empty");
        const int strangers = zox_tst_wfc_strangers(&patches, a, size);
        zox_tst_check(!strangers, "[%i] windows of the output aren't the source's patches of [%ix%ix%i]", strangers, n.x, n.y, n.z);
        wfc_patches_dispose(&patches);
    }
    // seeded
    const int3 n = wfc_patch_size;
    zox_tst_check(wfc_generate(source, s, a, size, 7, rules, n), "no output for seed 7");
    zox_tst_check(wfc_generate(source, s, b, size, 7, rules, n), "no output for seed 7, again");
    zox_tst_check(!memcmp(a, b, cells), "seed 7 made two different outputs");
    int differing = 0;
    for (uint seed = 8; seed < 12; seed++) {
        zox_tst_check(wfc_generate(source, s, b, size, seed, rules, n), "no output for seed [%u]", seed);
        differing += memcmp(a, b, cells) != 0;
    }
    zox_tst_check(differing == 4, "only [%i] of 4 other seeds made a different output", differing);
    // all the rules: a floor, mirrored on x, one piece of at least half the source
    zox_tst_check(wfc_generate(source, s, a, size, 3, wfc_rules_all, n), "no output with all the rules");
    int floor = 0;
    for (int x = 0; x < size.x; x++)
    for (int z = 0; z < size.z; z++) {
        floor += a[x + z * size.x * size.y] != 0;
    }
    zox_tst_check(floor > 0, "nothing on the floor");
    int unmirrored = 0;
    for (int z = 0; z < size.z; z++)
    for (int y = 0; y < size.y; y++)
    for (int x = 0; x < size.x; x++) {
        unmirrored += a[x + y * size.x + z * size.x * size.y] != a[size.x - 1 - x + y * size.x + z * size.x * size.y];
    }
    zox_tst_check(!unmirrored, "[%i] voxels differ from their mirror", unmirrored);
    const int solid = zox_tst_wfc_solid(a, size);
    zox_tst_check(solid * 2 >= source_solid, "one piece kept [%i] voxels, under half the source's [%i]", solid, source_solid);
    memcpy(b, a, cells);
    byte grounded;
    zox_tst_check(wfc_keep_one_piece(b, size, &grounded) == solid && grounded, "the output isn't one piece on the floor");
    free(a);
    free(b);
}
