// Wave function collapse (the overlapping model, in 3D): new voxel grids made from a source's patches (2 or 3 a side)
//  - a patch is a window of the source (air around it), the cell it's placed on gets its voxel at -offset (on a side
//      of 3 its middle, of 2 its lower end)
//  - two patches can be neighbours if they overlap: shifted a voxel, the slab they share is the same
//  - the least sure cell takes a patch (weighted by how often it's in the source), its neighbours lose the patches
//      that can't touch it, and so on, until every cell is sure; a contradiction starts again on the next seed
//  - grids are x fastest: x + y * size.x + z * size.x * size.y, y is up
//  - rules (wfc_rule_*) shape it: air outside the box, a floor, mirrored on x, one piece, anchored, shaped, feet
//  - 3 a side keeps the source's details but one small source can only make itself, 2 recombines it; chooks use
//      2 wide and 3 tall (wfc_patch_size), so a leg sees it goes on up (at 2 tall a leg could end in air like a toe)
#define wfc_rule_air_border 1   // the voxels outside the box are air, shapes close inside it
#define wfc_rule_floor 2        // the bottom row only takes patches from the source's lowest row
#define wfc_rule_mirror_x 4     // half is made and mirrored, its middle matching its own mirror
#define wfc_rule_one_piece 8    // keeps the biggest piece, dropped onto the floor; one under half the source is redone
#define wfc_rule_anchored 16    // a patch only goes near where it is in the source (a share of the box, wfc_anchor_slack)
#define wfc_rule_shaped 32      // a cell favours patches like the source's voxel at its share of the box (wfc_shape_weight)
#define wfc_rule_feet 64        // solid on the floor under the source's feet, and (one piece) it stands on them
#define wfc_rule_feet_middle 128 // with feet: only under the middle of the source's feet (tried when all of them fail)
#define wfc_rules_all 127
#define wfc_anchor_slack 0.1f
#define wfc_shape_bias 1000
#define wfc_tries 24
#define wfc_n_max 3
#define wfc_patch_size (int3) { 2, 3, 2 }    // the nodes' patches

typedef struct {
    const byte* source;
    int3 source_size;
    int3 n;             // a patch's sides, 2 or 3
    int3 offset;        // a patch's corner from its cell, -(n - 1) / 2
    int volume;         // n.x * n.y * n.z
    int middle;         // the index of its cell's voxel
    int count;
    int words;          // uint64_t a bitset of patches
    byte* voxels;       // volume a patch, x + y * n.x + z * n.x * n.y
    float* weights;
    byte* floor;        // placed on the source's lowest solid row
    int floor_y;
    float* place;       // 6 a patch: the lowest and highest share of the source's box it's placed on, per axis
    int* slab_lo[3];    // the id of a patch's lower slab on an axis (its side less the top layer)
    int* slab_hi[3];    // and its upper (less the bottom layer)
    int slabs[3];
    uint64_t* with_lo[3];   // a bitset per slab id: the patches whose lower slab it is
    uint64_t* with_hi[3];   // and whose upper slab it is
} wfc_patches;

static inline byte wfc_get(const byte* grid, int3 size, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z) {
        return 0;
    }
    return grid[x + y * size.x + z * size.x * size.y];
}

// a patch index's place in it on an axis
static inline int wfc_axis(const wfc_patches* p, int i, int axis) {
    return axis == 0 ? i % p->n.x : axis == 1 ? (i / p->n.x) % p->n.y : i / (p->n.x * p->n.y);
}

static inline uint wfc_hash(const byte* key, int length) {
    uint h = 2166136261u;
    for (int i = 0; i < length; i++) {
        h = (h ^ key[i]) * 16777619u;
    }
    return h;
}

// finds key in a table of ids (-1 empty) into keys, or adds it; returns its id
static int wfc_intern(int* table, int table_size, byte* keys, int* count, const byte* key, int length) {
    uint slot = wfc_hash(key, length) & (table_size - 1);
    while (table[slot] >= 0) {
        if (!memcmp(keys + table[slot] * length, key, length)) {
            return table[slot];
        }
        slot = (slot + 1) & (table_size - 1);
    }
    memcpy(keys + *count * length, key, length);
    table[slot] = *count;
    return (*count)++;
}

static inline int wfc_table_size(int count) {
    int size = 64;
    while (size < count * 2) {
        size *= 2;
    }
    return size;
}

// the source's n x n x n patches, placed on every voxel of it and the air shell around it
wfc_patches wfc_patches_make(const byte* source, int3 size, int3 n) {
    n = (int3) { int_max(2, int_min(n.x, wfc_n_max)), int_max(2, int_min(n.y, wfc_n_max)), int_max(2, int_min(n.z, wfc_n_max)) };
    wfc_patches p = { .source = source, .source_size = size, .n = n, .volume = n.x * n.y * n.z };
    p.offset = (int3) { -(n.x - 1) / 2, -(n.y - 1) / 2, -(n.z - 1) / 2 };
    p.middle = -p.offset.x - p.offset.y * n.x - p.offset.z * n.x * n.y;
    p.floor_y = size.y;
    for (int i = 0; i < size.x * size.y * size.z; i++) {
        if (source[i]) {
            p.floor_y = int_min(p.floor_y, (i / size.x) % size.y);
        }
    }
    const int most = (size.x + 2) * (size.y + 2) * (size.z + 2);
    const int table_size = wfc_table_size(most);
    int* table = malloc(sizeof(int) * table_size);
    memset(table, -1, sizeof(int) * table_size);
    p.voxels = malloc(p.volume * most);
    int* counts = calloc(most, sizeof(int));
    p.floor = calloc(most, 1);
    p.place = malloc(sizeof(float) * 6 * most);
    byte patch[wfc_n_max * wfc_n_max * wfc_n_max];
    // cells from the shell below to the shell above: a patch on each reaches the source
    for (int cz = -1; cz <= size.z; cz++)
    for (int cy = -1; cy <= size.y; cy++)
    for (int cx = -1; cx <= size.x; cx++) {
        for (int i = 0; i < p.volume; i++) {
            patch[i] = wfc_get(source, size, cx + wfc_axis(&p, i, 0) + p.offset.x, cy + wfc_axis(&p, i, 1) + p.offset.y,
                cz + wfc_axis(&p, i, 2) + p.offset.z);
        }
        const int id = wfc_intern(table, table_size, p.voxels, &p.count, patch, p.volume);
        const float share[3] = {
            (cx + 0.5f) / size.x,
            (cy + 0.5f) / size.y,
            (cz + 0.5f) / size.z };
        float* place = p.place + id * 6;
        for (int axis = 0; axis < 3; axis++) {
            place[axis] = counts[id] ? fminf(place[axis], share[axis]) : share[axis];
            place[axis + 3] = counts[id] ? fmaxf(place[axis + 3], share[axis]) : share[axis];
        }
        counts[id]++;
        if (cy == p.floor_y) {
            p.floor[id] = 1;
        }
    }
    free(table);
    p.words = (p.count + 63) / 64;
    p.weights = malloc(sizeof(float) * p.count);
    for (int i = 0; i < p.count; i++) {
        p.weights[i] = sqrtf((float) counts[i]);   // damped, or air (most of a box) drowns the rest
    }
    free(counts);
    // slabs: the voxels two patches share when one is a step along the axis from the other
    const int slab_table_size = wfc_table_size(p.count * 2);
    table = malloc(sizeof(int) * slab_table_size);
    byte* keys = malloc(p.volume * p.count * 2);
    byte slab[wfc_n_max * wfc_n_max * wfc_n_max];
    for (int axis = 0; axis < 3; axis++) {
        const int side = axis == 0 ? n.x : axis == 1 ? n.y : n.z;
        const int slab_length = p.volume / side * (side - 1);
        memset(table, -1, sizeof(int) * slab_table_size);
        p.slabs[axis] = 0;
        p.slab_lo[axis] = malloc(sizeof(int) * p.count);
        p.slab_hi[axis] = malloc(sizeof(int) * p.count);
        for (int k = 0; k < p.count; k++) {
            for (byte upper = 0; upper < 2; upper++) {
                int length = 0;
                for (int i = 0; i < p.volume; i++) {
                    const int at = wfc_axis(&p, i, axis);
                    if (upper ? at >= 1 : at <= side - 2) {
                        slab[length++] = p.voxels[k * p.volume + i];
                    }
                }
                const int id = wfc_intern(table, slab_table_size, keys, &p.slabs[axis], slab, slab_length);
                (upper ? p.slab_hi : p.slab_lo)[axis][k] = id;
            }
        }
        p.with_lo[axis] = calloc((size_t) p.slabs[axis] * p.words, sizeof(uint64_t));
        p.with_hi[axis] = calloc((size_t) p.slabs[axis] * p.words, sizeof(uint64_t));
        for (int k = 0; k < p.count; k++) {
            p.with_lo[axis][p.slab_lo[axis][k] * p.words + k / 64] |= 1ull << (k % 64);
            p.with_hi[axis][p.slab_hi[axis][k] * p.words + k / 64] |= 1ull << (k % 64);
        }
    }
    free(table);
    free(keys);
    return p;
}

void wfc_patches_dispose(wfc_patches* p) {
    free(p->voxels);
    free(p->weights);
    free(p->floor);
    free(p->place);
    for (int axis = 0; axis < 3; axis++) {
        free(p->slab_lo[axis]);
        free(p->slab_hi[axis]);
        free(p->with_lo[axis]);
        free(p->with_hi[axis]);
    }
}

// the source's voxel at a cell's share of the box (its middle's, as anchored), at row y unless it's -1
static inline byte wfc_source_at(const wfc_patches* p, int3 cell, int3 full, int y) {
    const int3 ss = p->source_size;
    return wfc_get(p->source, ss,
        (int) ((cell.x + 0.5f) / full.x * ss.x),
        y >= 0 ? y : (int) ((cell.y + 0.5f) / full.y * ss.y),
        (int) ((cell.z + 0.5f) / full.z * ss.z));
}

// whether patch k may sit on a cell: near where it is in the source, a floor one on the floor, what of it lies
//  outside the box air, and what lies past the mirror's line the same as its mirror
//  - size: the cells made (the lower half of full, mirrored)
static byte wfc_patch_fits(const wfc_patches* p, int k, int3 cell, int3 size, int3 full, byte rules) {
    const byte* v = p->voxels + k * p->volume;
    if (rules & wfc_rule_anchored) {
        const float share[3] = {
            (cell.x + 0.5f) / full.x,
            (cell.y + 0.5f) / full.y,
            (cell.z + 0.5f) / full.z };
        const float* place = p->place + k * 6;
        for (int axis = 0; axis < 3; axis++) {
            if (share[axis] < place[axis] - wfc_anchor_slack || share[axis] > place[axis + 3] + wfc_anchor_slack) {
                return 0;
            }
        }
    }
    if (rules & wfc_rule_floor && cell.y == 0 && !p->floor[k]) {
        return 0;
    }
    // feet: solid where the source's feet are on its floor row, or (feet middle) only their middles (solid there and
    //  at their 4 sides), the edges and toes left to the patches: a squashed or stretched foot may fit no patches
    if (rules & wfc_rule_feet && cell.y == 0 && !v[p->middle]) {
        byte middle = wfc_source_at(p, cell, full, p->floor_y) != 0;
        for (int side = 0; side < 4 && middle && rules & wfc_rule_feet_middle; side++) {
            const int3 beside = { cell.x + (side == 0) - (side == 1), 0, cell.z + (side == 2) - (side == 3) };
            middle = wfc_source_at(p, beside, full, p->floor_y) != 0;
        }
        if (middle) {
            return 0;
        }
    }
    const byte mirror = rules & wfc_rule_mirror_x;
    for (int i = 0; i < p->volume; i++) {
        const int3 local = { wfc_axis(p, i, 0), wfc_axis(p, i, 1), wfc_axis(p, i, 2) };
        const int3 at = { cell.x + local.x + p->offset.x, cell.y + local.y + p->offset.y, cell.z + local.z + p->offset.z };
        if (mirror && at.x >= size.x && at.x < full.x) {
            // the other half: the same as the voxel it mirrors, if this patch holds it
            const int mirrored = full.x - 1 - at.x - cell.x - p->offset.x;
            if (mirrored >= 0 && mirrored < p->n.x && v[i] != v[i - local.x + mirrored]) {
                return 0;
            }
            continue;
        }
        const byte outside = at.x < 0 || at.y < 0 || at.z < 0 || at.x >= full.x || at.y >= size.y || at.z >= size.z;
        if (rules & wfc_rule_air_border && outside && v[i]) {
            return 0;
        }
    }
    return 1;
}

// shaped: how much more a patch is picked for the source's voxel at the cell's share of the box (wanted)
//  - the same voxel most, then solid where it's solid (or air where air), then anything
static inline float wfc_shape_weight(const wfc_patches* p, int k, byte wanted, byte rules) {
    const byte voxel = p->voxels[k * p->volume + p->middle];
    if (!(rules & wfc_rule_shaped)) {
        return 1;
    }
    return voxel == wanted ? wfc_shape_bias : (voxel != 0) == (wanted != 0) ? wfc_shape_bias / 30 : 1;
}

// a try's state: each cell's patches left (wave), and for each side of it how many of them have each slab there
//  (support): a patch beside it stays while one of this cell's patches shares its slab
typedef struct {
    const wfc_patches* p;
    int3 size;
    uint64_t* wave;
    int* remaining;
    float* sum;         // the weights of its patches left, for its entropy
    float* sum_log;
    ushort* support;    // [cell][axis * 2 + upper][slab]
    int slabs;
    int* queue;         // patches removed, their neighbours not yet told: cell * count + patch
    int queued;
    byte failed;
} wfc_state;

static inline ushort* wfc_support(wfc_state* s, int c, int axis, byte upper) {
    return s->support + ((size_t) c * 6 + axis * 2 + upper) * s->slabs;
}

static inline float wfc_weight_log(float w) {
    return w * logf(w);
}

static void wfc_remove(wfc_state* s, int c, int k) {
    const int words = s->p->words;
    s->wave[(size_t) c * words + k / 64] &= ~(1ull << (k % 64));
    s->sum[c] -= s->p->weights[k];
    s->sum_log[c] -= wfc_weight_log(s->p->weights[k]);
    if (--s->remaining[c] == 0) {
        s->failed = 1;
    }
    s->queue[s->queued++] = c * s->p->count + k;
}

// a neighbour's step along an axis, or -1 past the box
static inline int wfc_neighbour(wfc_state* s, int c, int axis, byte upper) {
    const int3 size = s->size;
    const int at = axis == 0 ? c % size.x : axis == 1 ? (c / size.x) % size.y : c / (size.x * size.y);
    const int length = axis == 0 ? size.x : axis == 1 ? size.y : size.z;
    const int step = axis == 0 ? 1 : axis == 1 ? size.x : size.x * size.y;
    if (upper ? at + 1 >= length : at == 0) {
        return -1;
    }
    return c + (upper ? step : -step);
}

// removes the patches of n (beside c, its upper side if upper) a slab of c no longer has: with[slab] in n
static void wfc_unsupport(wfc_state* s, int n, const uint64_t* with) {
    const int words = s->p->words;
    uint64_t* bits = s->wave + (size_t) n * words;
    for (int w = 0; w < words; w++) {
        uint64_t b = bits[w] & with[w];
        while (b) {
            const int j = w * 64 + __builtin_ctzll(b);
            b &= b - 1;
            wfc_remove(s, n, j);
        }
    }
}

static void wfc_propagate(wfc_state* s) {
    const wfc_patches* p = s->p;
    while (s->queued && !s->failed) {
        const int q = s->queue[--s->queued];
        const int c = q / p->count;
        const int k = q % p->count;
        for (int axis = 0; axis < 3; axis++) {
            for (byte upper = 0; upper < 2; upper++) {
                // above: their lower slab must be one of our upper ones; below: their upper one of our lower
                const int slab = (upper ? p->slab_hi : p->slab_lo)[axis][k];
                ushort* count = wfc_support(s, c, axis, upper) + slab;
                if (--(*count)) {
                    continue;
                }
                const int n = wfc_neighbour(s, c, axis, upper);
                if (n >= 0) {
                    wfc_unsupport(s, n, (upper ? p->with_lo : p->with_hi)[axis] + (size_t) slab * p->words);
                }
            }
        }
    }
}

// one try at filling cells (size) with patches, 0 on a contradiction
static byte wfc_collapse(const wfc_patches* p, int3 size, int3 full, uint seed, byte rules, int* chosen) {
    const int words = p->words;
    const int cells = size.x * size.y * size.z;
    wfc_state s = { .p = p, .size = size };
    s.slabs = int_max(p->slabs[0], int_max(p->slabs[1], p->slabs[2]));
    s.wave = calloc((size_t) words * cells, sizeof(uint64_t));
    s.remaining = calloc(cells, sizeof(int));
    s.sum = calloc(cells, sizeof(float));
    s.sum_log = calloc(cells, sizeof(float));
    s.support = calloc((size_t) cells * 6 * s.slabs, sizeof(ushort));
    s.queue = malloc(sizeof(int) * ((size_t) cells * p->count + 1));
    // what fits each cell: inside, all of them; on the box's sides, floor and mirror only some
    for (int c = 0; c < cells; c++) {
        const int3 cell = { c % size.x, (c / size.x) % size.y, c / (size.x * size.y) };
        const byte inside = !(rules & wfc_rule_anchored) && cell.x > 0 && cell.y > 0 && cell.z > 0 && cell.x < size.x - 1 && cell.y < size.y - 1 &&
            cell.z < size.z - 1;
        uint64_t* bits = s.wave + (size_t) c * words;
        for (int k = 0; k < p->count; k++) {
            if (inside || wfc_patch_fits(p, k, cell, size, full, rules)) {
                bits[k / 64] |= 1ull << (k % 64);
                s.remaining[c]++;
                s.sum[c] += p->weights[k];
                s.sum_log[c] += wfc_weight_log(p->weights[k]);
                for (int axis = 0; axis < 3; axis++) {
                    wfc_support(&s, c, axis, 0)[p->slab_lo[axis][k]]++;
                    wfc_support(&s, c, axis, 1)[p->slab_hi[axis][k]]++;
                }
            }
        }
        s.failed |= !s.remaining[c];
    }
    // patches with no support from a neighbour go
    for (int c = 0; c < cells && !s.failed; c++) {
        for (int axis = 0; axis < 3; axis++) {
            for (byte upper = 0; upper < 2; upper++) {
                const int n = wfc_neighbour(&s, c, axis, upper);
                if (n < 0) {
                    continue;
                }
                const ushort* support = wfc_support(&s, c, axis, upper);
                const int* theirs = (upper ? p->slab_lo : p->slab_hi)[axis];
                uint64_t* bits = s.wave + (size_t) n * words;
                for (int k = 0; k < p->count; k++) {
                    if ((bits[k / 64] >> (k % 64)) & 1 && !support[theirs[k]]) {
                        wfc_remove(&s, n, k);
                    }
                }
            }
        }
    }
    wfc_propagate(&s);
    while (!s.failed) {
        // the least sure cell left (a seeded nudge breaks ties)
        int best = -1;
        float lowest = 0;
        for (int c = 0; c < cells; c++) {
            if (s.remaining[c] <= 1) {
                continue;
            }
            const float entropy = logf(s.sum[c]) - s.sum_log[c] / s.sum[c] +
                (seed_rand(seed ^ (uint) c * 2654435761u) & 1023) * 0.000001f;
            if (best < 0 || entropy < lowest) {
                best = c;
                lowest = entropy;
            }
        }
        if (best < 0) {
            break;
        }
        // it takes one of its patches, by their weights (shaped: the source's voxel at its share of the box favoured)
        uint64_t* bits = s.wave + (size_t) best * words;
        const int3 cell = { best % size.x, (best / size.x) % size.y, best / (size.x * size.y) };
        const byte wanted = wfc_source_at(p, cell, full, -1);
        float total = 0;
        for (int k = 0; k < p->count; k++) {
            if ((bits[k / 64] >> (k % 64)) & 1) {
                const float bias = wfc_shape_weight(p, k, wanted, rules);
                total += p->weights[k] * bias;
            }
        }
        seed = seed_rand(seed + 0x9E3779B9u);
        float roll = (float) seed / (float) UINT_MAX * total;
        int pick = -1;
        for (int k = 0; k < p->count; k++) {
            if ((bits[k / 64] >> (k % 64)) & 1) {
                const float bias = wfc_shape_weight(p, k, wanted, rules);
                pick = k;
                roll -= p->weights[k] * bias;
                if (roll <= 0) {
                    break;
                }
            }
        }
        for (int k = 0; k < p->count; k++) {
            if (k != pick && (bits[k / 64] >> (k % 64)) & 1) {
                wfc_remove(&s, best, k);
            }
        }
        wfc_propagate(&s);
    }
    for (int c = 0; c < cells && !s.failed; c++) {
        const uint64_t* bits = s.wave + (size_t) c * words;
        chosen[c] = -1;
        for (int w = 0; w < words && chosen[c] < 0; w++) {
            if (bits[w]) {
                chosen[c] = w * 64 + __builtin_ctzll(bits[w]);
            }
        }
    }
    const byte made = !s.failed;
    free(s.wave);
    free(s.remaining);
    free(s.sum);
    free(s.sum_log);
    free(s.support);
    free(s.queue);
    return made;
}

// keeps the biggest piece of out (touching by a face, edge or corner) and drops it onto the floor, returns its voxels
//  - grounded: whether it reached the floor before it was dropped (it stood on its feet)
static int wfc_keep_one_piece(byte* out, int3 size, byte* grounded) {
    const int cells = size.x * size.y * size.z;
    int* piece = calloc(cells, sizeof(int));
    int* queue = malloc(sizeof(int) * cells);
    int best = 0;
    int best_count = 0;
    int best_low = 0;
    int pieces = 0;
    for (int c = 0; c < cells; c++) {
        if (!out[c] || piece[c]) {
            continue;
        }
        pieces++;
        int head = 0;
        int tail = 0;
        int low = size.y;
        queue[tail++] = c;
        piece[c] = pieces;
        while (head < tail) {
            const int at = queue[head++];
            const int3 cell = { at % size.x, (at / size.x) % size.y, at / (size.x * size.y) };
            low = int_min(low, cell.y);
            for (int d = 0; d < 27; d++) {
                const int3 n = { cell.x + d % 3 - 1, cell.y + (d / 3) % 3 - 1, cell.z + d / 9 - 1 };
                if (!wfc_get(out, size, n.x, n.y, n.z)) {
                    continue;
                }
                const int index = n.x + n.y * size.x + n.z * size.x * size.y;
                if (!piece[index]) {
                    piece[index] = pieces;
                    queue[tail++] = index;
                }
            }
        }
        if (tail > best_count) {
            best = pieces;
            best_count = tail;
            best_low = low;
        }
    }
    const int layer = size.x;
    for (int z = 0; z < size.z; z++)
    for (int y = 0; y < size.y; y++)
    for (int x = 0; x < size.x; x++) {
        const int c = x + y * layer + z * layer * size.y;
        const int above = y + best_low < size.y ? c + best_low * layer : -1;
        out[c] = above >= 0 && piece[above] == best ? out[above] : 0;
    }
    free(piece);
    free(queue);
    *grounded = best_count && !best_low;
    return best_count;
}

// makes out (size) from source's patches (n, 2 or 3 a side); returns 1 if it did, else out is left empty
byte wfc_generate(const byte* source, int3 source_size, byte* out, int3 size, uint seed, byte rules, int3 n) {
    const int cells_out = size.x * size.y * size.z;
    memset(out, 0, cells_out);
    int source_solid = 0;
    for (int i = 0; i < source_size.x * source_size.y * source_size.z; i++) {
        source_solid += source[i] != 0;
    }
    if (!source_solid || !cells_out) {
        return 0;
    }
    wfc_patches p = wfc_patches_make(source, source_size, n);
    // mirrored, the made half is the lower x, the line on its last column (odd) or after it (even)
    const int3 half = { rules & wfc_rule_mirror_x ? (size.x + 1) / 2 : size.x, size.y, size.z };
    const int cells = half.x * half.y * half.z;
    int* chosen = malloc(sizeof(int) * cells);
    byte made = 0;
    for (int t = 0; t < wfc_tries && !made; t++) {
        if (!wfc_collapse(&p, half, size, seed_rand(seed + t * 7919u), rules, chosen)) {
            continue;
        }
        for (int c = 0; c < cells; c++) {
            const int3 cell = { c % half.x, (c / half.x) % half.y, c / (half.x * half.y) };
            const byte voxel = p.voxels[chosen[c] * p.volume + p.middle];
            out[cell.x + cell.y * size.x + cell.z * size.x * size.y] = voxel;
            if (rules & wfc_rule_mirror_x) {
                const int mirror = size.x - 1 - cell.x;
                out[mirror + cell.y * size.x + cell.z * size.x * size.y] = voxel;
            }
        }
        made = 1;
        byte grounded;
        if (rules & wfc_rule_one_piece && (wfc_keep_one_piece(out, size, &grounded) * 2 < source_solid ||
            (rules & wfc_rule_feet && !grounded))) {
            memset(out, 0, cells_out);
            made = 0;
        }
    }
    free(chosen);
    wfc_patches_dispose(&p);
    // never made: with less of the feet, then the source stretched to the box (on its feet, rather than a block);
    //  without feet, in pieces rather than nothing
    if (!made && rules & wfc_rule_feet && !(rules & wfc_rule_feet_middle)) {
        return wfc_generate(source, source_size, out, size, seed, rules | wfc_rule_feet_middle, n);
    }
    if (!made && rules & wfc_rule_feet) {
        for (int c = 0; c < cells_out; c++) {
            const int3 cell = { c % size.x, (c / size.x) % size.y, c / (size.x * size.y) };
            out[c] = wfc_get(source, source_size, cell.x * source_size.x / size.x, cell.y * source_size.y / size.y,
                cell.z * source_size.z / size.z);
        }
        return 1;
    }
    if (!made && rules & wfc_rule_one_piece) {
        return wfc_generate(source, source_size, out, size, seed, rules & ~wfc_rule_one_piece, n);
    }
    return made;
}
