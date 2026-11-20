// tst: vegetation_across, a tree crossing two chunks stacked vertically is drawn whole (vegetation_draw_tree)
//  - each trunk height (3 to 5), the ground at every height near the lower chunk's top: each chunk draws its part
//  - together: wood at every trunk height, leaves over its top, every canopy block, nothing twice, nothing else
//  - no world needed: the in game test (vegetation_chunks) found a crossing tree by luck, 1 run in 3
//  - run: bash bsh/tst.sh vegetation_across
#define zox_tst_va_depth 4
#define zox_tst_va_wood 1
#define zox_tst_va_leaves 2

// a global height's voxel, from the chunk holding it (below 0 to length, above length to 2 * length)
static inline byte zox_tst_va_at(VoxelNode* chunks, byte3 p, int y, byte length) {
    if (y < 0 || y >= 2 * length) {
        return 0;
    }
    const byte3 local = { p.x, y % length, p.z };
    return getv_VoxelNode(&chunks[y / length], zox_tst_va_depth, local);
}

void zox_tst_vegetation_across_check(ecs* world) {
    (void) world;
    const byte length = octree_size(zox_tst_va_depth);
    const byte3 position = { length / 2, 0, length / 2 };
    int canopy = 0;
    for (int dx = -zox_tree_canopy; dx <= zox_tree_canopy; dx++)
    for (int dz = -zox_tree_canopy; dz <= zox_tree_canopy; dz++)
    for (int dy = -1; dy <= 1; dy++) {
        canopy += is_canopy(dx, dy, dz);
    }
    int cases = 0, crossing = 0, no_wood = 0, no_leaves = 0, wrong_count = 0;
    for (int trunk = 3; trunk <= 5; trunk++)
    for (int height = length - 8; height < length; height++) {
        VoxelNode chunks[2] = { { 0 }, { 0 } };
        for (byte c = 0; c < 2; c++) {
            vegetation_draw_tree(&chunks[c], zox_tst_va_depth, position, c * length, 1, height, trunk,
                zox_tst_va_wood, zox_tst_va_leaves);
        }
        const int top = height + trunk;
        cases++;
        crossing += (height + 1) / length != (top + 1) / length;
        for (int h = 1; h <= trunk; h++) {
            no_wood += zox_tst_va_at(chunks, position, height + h, length) != zox_tst_va_wood;
        }
        no_leaves += zox_tst_va_at(chunks, position, top + 1, length) != zox_tst_va_leaves;
        // every block of both chunks: trunk + canopy, nothing more
        int woods = 0, leaves = 0;
        for (int y = 0; y < 2 * length; y++)
        for (byte x = 0; x < length; x++)
        for (byte z = 0; z < length; z++) {
            const byte v = zox_tst_va_at(chunks, (byte3) { x, 0, z }, y, length);
            woods += v == zox_tst_va_wood;
            leaves += v == zox_tst_va_leaves;
        }
        if (woods != trunk || leaves != canopy) {
            if (!wrong_count) {
                zox_log("[tst] ground [%i] trunk [%i]: [%i] wood (not %i), [%i] leaves (not %i)", height, trunk,
                    woods, trunk, leaves, canopy);
            }
            wrong_count++;
        }
        collapse_VoxelNode(&chunks[0]);
        collapse_VoxelNode(&chunks[1]);
    }
    zox_log("[tst] [metric] vegetation_across [%i] trees, [%i] crossing the chunks, a canopy of [%i] leaves",
        cases, crossing, canopy);
    zox_tst_check(crossing > 0, "no case crosses the chunks");
    zox_tst_check(!no_wood, "[%i] trunk blocks missing", no_wood);
    zox_tst_check(!no_leaves, "[%i] trees have no leaves over their trunk", no_leaves);
    zox_tst_check(!wrong_count, "[%i] of [%i] trees aren't whole across the chunks", wrong_count, cases);
}
