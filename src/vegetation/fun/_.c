// Where vegetation goes, as functions of a column (vegetation/sys, tst vegetation_trees, vegetation_cover)
//  - trees: a jittered grid, a cell of zox_tree_cell blocks has a tree with the biome's TreeChance, at a seeded
//    spot zox_tree_margin from its edges, so trees are apart (they came from the top of a noise, rare and in clumps)
//    - cells line up with tunks (zox_tree_cell divides a chunk's length) and a canopy reaches zox_tree_canopy,
//      less than the margin, so a tree and its leaves stay in its tunk, a chunk draws whole trees
//  - ground cover: the biome's shares (DirtChance, GrassChance, WeedsChance tall grass, FlowerChance), picked by a
//    seeded number a column; tall grass and flowers grow in clumps: low noise scales their shares from none to
//    twice (the average holds), open grass between (all one mass of tall grass looked unnatural)
#define zox_tree_cell 8
#define zox_tree_margin 2
#define zox_tree_canopy 2
#define zox_vegetation_clump 6          // blocks between a clump field's points: clumps a few blocks across
#define zox_vegetation_clump_sharpness 3.0f  // how sharp a clump's edge is (1 soft, higher all or nothing)

static inline uint vegetation_hash(int x, int y, lint seed, uint salt) {
    uint h = (uint) x * 374761393u + (uint) y * 668265263u + (uint) seed * 2246822519u + salt * 3266489917u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

static inline float vegetation_random01(int x, int y, lint seed, uint salt) {
    return (vegetation_hash(x, y, seed, salt) & 0xffffff) / (float) 0x1000000;
}

static inline int vegetation_floor_div(int a, int b) {
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

// the tree in a column's cell, 0 if the cell has none
static inline byte get_cell_tree(int2 global_position, lint seed, float tree_chance, int2* tree) {
    const int cx = vegetation_floor_div(global_position.x, zox_tree_cell);
    const int cy = vegetation_floor_div(global_position.y, zox_tree_cell);
    if (vegetation_random01(cx, cy, seed, 1) >= tree_chance) {
        return 0;
    }
    const int span = zox_tree_cell - 2 * zox_tree_margin;
    tree->x = cx * zox_tree_cell + zox_tree_margin + (int) (vegetation_hash(cx, cy, seed, 2) % span);
    tree->y = cy * zox_tree_cell + zox_tree_margin + (int) (vegetation_hash(cx, cy, seed, 3) % span);
    return 1;
}

static inline byte is_tree_at(int2 global_position, lint seed, float tree_chance) {
    int2 tree;
    return get_cell_tree(global_position, seed, tree_chance, &tree) &&
        tree.x == global_position.x && tree.y == global_position.y;
}

// a tree's trunk height, seeded by its column (rand() wasn't the same each time, nor thread safe)
static inline byte get_tree_height(int2 global_position, lint seed) {
    return 3 + vegetation_hash(global_position.x, global_position.y, seed, 4) % 3;
}

// leaves around a trunk's top: offset from the top, a rounded blob
static inline byte is_canopy(int dx, int dy, int dz) {
    if (dx == 0 && dz == 0 && dy < 1) {
        return 0;   // the trunk
    }
    return dx * dx + dz * dz + 2 * dy * dy <= zox_tree_canopy * zox_tree_canopy + 1 && dy >= -1 && dy <= 1;
}

// a clump field, 0 to 1 and even about a half (so shares scaled by it hold on average): seeded values on a grid
//  zox_vegetation_clump apart, smoothly blended between
static inline float vegetation_clump_field(int2 p, lint seed) {
    const int cx = vegetation_floor_div(p.x, zox_vegetation_clump), cy = vegetation_floor_div(p.y, zox_vegetation_clump);
    float tx = (p.x - cx * zox_vegetation_clump) / (float) zox_vegetation_clump;
    float ty = (p.y - cy * zox_vegetation_clump) / (float) zox_vegetation_clump;
    tx = tx * tx * (3 - 2 * tx);
    ty = ty * ty * (3 - 2 * ty);
    const float a = vegetation_random01(cx, cy, seed, 6), b = vegetation_random01(cx + 1, cy, seed, 6);
    const float c = vegetation_random01(cx, cy + 1, seed, 6), d = vegetation_random01(cx + 1, cy + 1, seed, 6);
    return (a + (b - a) * tx) * (1 - ty) + (c + (d - c) * tx) * ty;
}

// a column's ground cover from the biome's shares: dirt, grass, weeds (tall grass), flowers, the rest grass
static inline byte get_ground_cover(int2 global_position, byte terrain_length, lint seed, const float shares[4]) {
    (void) terrain_length;
    // clumps: tall grass and flowers from none to twice their share (symmetric about 1, the shares hold), the rest grass
    const float clump = clampf(1 + (vegetation_clump_field(global_position, seed) - 0.5f) * 2 * zox_vegetation_clump_sharpness, 0, 2);
    // grass last, what's left: in a clump the others take more of the column, never past it
    const float local[3] = { shares[0], shares[2] * clump, shares[3] * clump };
    const float u = vegetation_random01(global_position.x, global_position.y, seed, 5);
    const byte covers[3] = { zox_vegetation_dirt, zox_vegetation_weeds, zox_vegetation_flowers };
    float sum = 0;
    for (byte i = 0; i < 3; i++) {
        sum += local[i];
        if (u < sum) {
            return covers[i];
        }
    }
    return zox_vegetation_grass;
}
