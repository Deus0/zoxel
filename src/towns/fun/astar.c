// A* over a grid of the ground: a path round what's in its way, keeping to gentle ground; kept as a tool, not used by
//  roads (towns/fun/town.c's waypoint graph and fun/highways.c's lattice are cheap enough to make every tunk's): it
//  samples the terrain over its whole grid, 1 to 19ms a town or region (metrics.md), fine for a few paths, not for
//  every chunk of a world
//  - a grid of cells over an area, each with its ground's height and a cost: water and mountains dear, blocked cells
//    (path_blocked) never crossed
//  - diagonal steps too; a climb costs its height squared, so a path goes round a hill, not up it; a cell an earlier
//    path took costs less (path_grid_road), so paths join
//  - straightened: from each point the farthest later one in a clear line (nothing dearer than its ends, the ground
//    within path_bump of an even slope between them)
//  - tst town_build checks it: a straight line over open ground, through a wall's gap, round a hill
#define path_cells_max 32768
#define path_points_max 256
#define path_blocked 255
#define path_cost_ground 4      // a cell's cost: ground 4, a road's cell 1 (roads share), more for water, mountains
#define path_cost_road 1
#define path_bump 2             // blocks the ground may rise or fall off a straightened segment's slope

typedef struct {
    int2 origin;    // its first cell's low corner, in blocks
    int cell;       // blocks a side
    int2 size;      // cells
    short heights[path_cells_max];
    byte costs[path_cells_max];
} PathGrid;

// a grid over an area of blocks, its cells cell blocks a side (doubled until it fits)
static inline void path_grid_begin(PathGrid* g, int2 origin, int2 blocks, int cell) {
    while (((blocks.x + cell - 1) / cell) * ((blocks.y + cell - 1) / cell) > path_cells_max) {
        cell *= 2;
    }
    g->origin = origin;
    g->cell = cell;
    g->size = (int2) { (blocks.x + cell - 1) / cell, (blocks.y + cell - 1) / cell };
}

static inline int path_index(const PathGrid* g, int2 c) {
    return c.x + c.y * g->size.x;
}

static inline byte path_inside(const PathGrid* g, int2 c) {
    return c.x >= 0 && c.y >= 0 && c.x < g->size.x && c.y < g->size.y;
}

// the cell a block is in, and a cell's middle block
static inline int2 path_cell(const PathGrid* g, int2 block) {
    return (int2) { floor_div(block.x - g->origin.x, g->cell), floor_div(block.y - g->origin.y, g->cell) };
}

static inline int2 path_middle(const PathGrid* g, int2 c) {
    return (int2) { g->origin.x + c.x * g->cell + g->cell / 2, g->origin.y + c.y * g->cell + g->cell / 2 };
}

// each cell's ground from the terrain, water's top over water (a road over it is a causeway) costing water_cost
//  - sampled every every cells a side, the cells between sharing it (the terrain's height is its dearest part)
static inline void path_grid_terrain(PathGrid* g, const TerrainHeights* th, int sea, byte water_cost, int every) {
    for (int y = 0; y < g->size.y; y += every)
    for (int x = 0; x < g->size.x; x += every) {
        const int2 m = path_middle(g, (int2) { x + every / 2, y + every / 2 });
        const int ground = terrain_height_at(th, m);
        const int water = terrain_water_level_at(th, m, sea);
        for (int y2 = y; y2 < int_min(y + every, g->size.y); y2++)
        for (int x2 = x; x2 < int_min(x + every, g->size.x); x2++) {
            const int i = x2 + y2 * g->size.x;
            g->heights[i] = (short) int_max(ground, water);
            g->costs[i] = ground < water ? water_cost : path_cost_ground;
        }
    }
}

// raises the cost of the cells whose middles are in a rect (its middle, half its size, in blocks), to at least cost
//  - by middles, not any overlap: a gap of a cell's width between two rects keeps a lane (it closed doors in)
static inline void path_grid_rect(PathGrid* g, int2 center, int2 half, byte cost) {
    const int2 low = path_cell(g, int2_sub(center, half)), high = path_cell(g, int2_add(center, half));
    for (int y = int_max(low.y, 0); y <= int_min(high.y, g->size.y - 1); y++)
    for (int x = int_max(low.x, 0); x <= int_min(high.x, g->size.x - 1); x++) {
        const int2 m = path_middle(g, (int2) { x, y });
        if (abs(m.x - center.x) > half.x || abs(m.y - center.y) > half.y) {
            continue;
        }
        byte* c = &g->costs[x + y * g->size.x];
        *c = *c > cost ? *c : cost;
    }
}

// raises the cost of the cells whose middles are in a circle, to at least cost
static inline void path_grid_circle(PathGrid* g, int2 center, int radius, byte cost) {
    const int2 low = path_cell(g, int2_sub(center, int2_single(radius))), high = path_cell(g, int2_add(center, int2_single(radius)));
    for (int y = int_max(low.y, 0); y <= int_min(high.y, g->size.y - 1); y++)
    for (int x = int_max(low.x, 0); x <= int_min(high.x, g->size.x - 1); x++) {
        const int2 m = path_middle(g, (int2) { x, y });
        if ((m.x - center.x) * (m.x - center.x) + (m.y - center.y) * (m.y - center.y) <= radius * radius) {
            byte* c = &g->costs[x + y * g->size.x];
            *c = *c > cost ? *c : cost;
        }
    }
}

// a cell's cost (blocked ends aren't: a path leaves a door into its home's margin)
static inline int path_cost_at(const PathGrid* g, int i, int start, int goal) {
    return i == start || i == goal ? path_cost_ground : g->costs[i];
}

// its ground along the line from a to b (blocks) keeps within path_bump of their even slope, and crosses no cell
//  dearer than its ends' (a town's margin, water, a mountain: the found path's way round them is kept)
static inline byte path_line_clear(const PathGrid* g, int2 a, int ha, int2 b, int hb) {
    const int2 ca = path_cell(g, a), cb = path_cell(g, b);
    const int length = int_max(abs(b.x - a.x), abs(b.y - a.y));
    const int steps = int_max(1, (length * 2) / g->cell);
    int dearest = path_cost_ground;
    if (path_inside(g, ca) && g->costs[path_index(g, ca)] != path_blocked) dearest = int_max(dearest, g->costs[path_index(g, ca)]);
    if (path_inside(g, cb) && g->costs[path_index(g, cb)] != path_blocked) dearest = int_max(dearest, g->costs[path_index(g, cb)]);
    for (int s = 1; s < steps; s++) {
        const float t = (float) s / steps;
        const int2 p = { a.x + (int) roundf((b.x - a.x) * t), a.y + (int) roundf((b.y - a.y) * t) };
        const int2 c = path_cell(g, p);
        if (!path_inside(g, c)) {
            return 0;
        }
        const int i = path_index(g, c);
        if (int2_equals(c, ca) || int2_equals(c, cb)) {
            continue;
        }
        if (g->costs[i] > dearest || abs(g->heights[i] - (ha + (hb - ha) * t)) > path_bump) {
            return 0;
        }
    }
    return 1;
}

// a lower bound of a path's cost between cells: octile steps at a road's cost
static inline int path_guess(int2 a, int2 b) {
    const int dx = abs(a.x - b.x), dy = abs(a.y - b.y);
    return path_cost_road * (10 * int_max(dx, dy) + 4 * int_min(dx, dy));
}

// a path from a to b (blocks): its points (x, its ground, z) from a to b, straightened; their count, 0 if none
int path_find(const PathGrid* g, int2 a, int2 b, int3* points, int points_max) {
    const int2 ca = path_cell(g, a), cb = path_cell(g, b);
    if (!path_inside(g, ca) || !path_inside(g, cb) || points_max < 2) {
        return 0;
    }
    const int cells = g->size.x * g->size.y;
    const int start = path_index(g, ca), goal = path_index(g, cb);
    int* costs = zox_malloc(sizeof(int) * cells);
    int* from = zox_malloc(sizeof(int) * cells);
    int* heap = zox_malloc(sizeof(int) * cells * 8);
    int* heap_f = zox_malloc(sizeof(int) * cells * 8);
    for (int i = 0; i < cells; i++) {
        costs[i] = INT32_MAX;
        from[i] = -1;
    }
    // a binary heap of cells by their cost so far plus an octile guess to the goal
    int heap_count = 0;
    costs[start] = 0;
    heap[heap_count] = start;
    heap_f[heap_count++] = 0;
    const sbyte2 steps[8] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };
    byte found = 0;
    while (heap_count) {
        const int i = heap[0], f = heap_f[0];
        heap_count--;
        heap[0] = heap[heap_count];
        heap_f[0] = heap_f[heap_count];
        for (int k = 0;;) {
            const int l = k * 2 + 1, r = l + 1;
            int m = k;
            if (l < heap_count && heap_f[l] < heap_f[m]) m = l;
            if (r < heap_count && heap_f[r] < heap_f[m]) m = r;
            if (m == k) break;
            int t = heap[k]; heap[k] = heap[m]; heap[m] = t;
            t = heap_f[k]; heap_f[k] = heap_f[m]; heap_f[m] = t;
            k = m;
        }
        if (i == goal) {
            found = 1;
            break;
        }
        const int2 c = { i % g->size.x, i / g->size.x };
        if (f > costs[i] + path_guess(c, cb)) {
            continue;   // a stale entry
        }
        for (byte s = 0; s < 8; s++) {
            const int2 n = { c.x + steps[s].x, c.y + steps[s].y };
            if (!path_inside(g, n)) {
                continue;
            }
            const int j = path_index(g, n);
            const int cost = path_cost_at(g, j, start, goal);
            if (cost == path_blocked) {
                continue;
            }
            // a diagonal between two blocked cells is a gap a road can't fit through
            if (s >= 4 && (path_cost_at(g, path_index(g, (int2) { c.x + steps[s].x, c.y }), start, goal) == path_blocked ||
                path_cost_at(g, path_index(g, (int2) { c.x, c.y + steps[s].y }), start, goal) == path_blocked)) {
                continue;
            }
            const int climb = g->heights[j] - g->heights[i];
            const int next = costs[i] + (s < 4 ? 10 : 14) * cost + climb * climb * path_cost_ground;
            if (next >= costs[j]) {
                continue;
            }
            costs[j] = next;
            from[j] = i;
            if (heap_count < cells * 8) {
                int k = heap_count++;
                heap[k] = j;
                heap_f[k] = next + path_guess(n, cb);
                while (k && heap_f[(k - 1) / 2] > heap_f[k]) {
                    const int p = (k - 1) / 2;
                    int t = heap[k]; heap[k] = heap[p]; heap[p] = t;
                    t = heap_f[k]; heap_f[k] = heap_f[p]; heap_f[p] = t;
                    k = p;
                }
            }
        }
    }
    int count = 0;
    if (found) {
        // its cells from the goal back, then turned round: a to b, its ends the exact blocks
        int n = 0;
        for (int i = goal; i >= 0; i = from[i]) {
            n++;
        }
        int2* line = zox_malloc(sizeof(int2) * (n + 1));
        int* heights = zox_malloc(sizeof(int) * (n + 1));
        int k2 = n - 1;
        for (int i = goal; i >= 0; i = from[i]) {
            heap[k2--] = i;     // the heap's done with, it holds the cells a to b
        }
        for (int k = 0; k < n; k++) {
            const int2 c = { heap[k] % g->size.x, heap[k] / g->size.x };
            line[k] = k == 0 ? a : k == n - 1 ? b : path_middle(g, c);
            heights[k] = g->heights[path_index(g, c)];
        }
        if (n == 1) {
            line[1] = b;
            heights[1] = heights[0];
            n = 2;
        }
        // straightened: from each point, the farthest it sees clearly
        points[count++] = (int3) { line[0].x, heights[0], line[0].y };
        for (int k = 0; k < n - 1 && count < points_max;) {
            int farthest = k + 1;
            for (int m = n - 1; m > k + 1; m--) {
                if (path_line_clear(g, line[k], heights[k], line[m], heights[m])) {
                    farthest = m;
                    break;
                }
            }
            points[count++] = (int3) { line[farthest].x, heights[farthest], line[farthest].y };
            k = farthest;
        }
        zox_free(line);
        zox_free(heights);
    }
    zox_free(costs);
    zox_free(from);
    zox_free(heap);
    zox_free(heap_f);
    return count;
}

// a found path's cells cost a road's, so the next roads join it
static inline void path_grid_road(PathGrid* g, const int3* points, int count) {
    for (int k = 0; k + 1 < count; k++) {
        const int2 a = { points[k].x, points[k].z }, b = { points[k + 1].x, points[k + 1].z };
        const int length = int_max(abs(b.x - a.x), abs(b.y - a.y));
        const int steps = int_max(1, (length * 2) / g->cell);
        for (int s = 0; s <= steps; s++) {
            const float t = (float) s / steps;
            const int2 c = path_cell(g, (int2) { a.x + (int) roundf((b.x - a.x) * t), a.y + (int) roundf((b.y - a.y) * t) });
            if (path_inside(g, c) && g->costs[path_index(g, c)] == path_cost_ground) {
                g->costs[path_index(g, c)] = path_cost_road;
            }
        }
    }
}
