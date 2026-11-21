// Highways: roads between a region's towns, out to its borders where the next region's meet them
//  - each border has one crossing, from the terrain's seed and the border's place, so both regions sharing it pick
//    the same one (and a region with no towns still links its borders, through its middle)
//  - its towns join gate to gate, each to the nearest one already joined (a tree), each crossing to the nearest gate
//  - a highway leaves its ends straight out (from a gate, in from a border) for highway_out blocks, then two legs;
//    town_map_system marks it on ground no town holds, town_walls_system paves it, over water at the sea's top
#define highway_half 2          // 5 blocks wide
#define highway_out 12
#define highway_ends_max 16     // its towns' gates and its 4 crossings
#define highways_max 64         // legs: 4 a highway

typedef struct {
    int2 at;
    int2 out;       // the way it leaves, a unit along x or z
} HighwayEnd;

typedef struct {
    TownRoad legs[highways_max];
    byte count;
} Highways;

// a border's crossing, along it from start (size blocks long), the same for both its regions
static inline int highway_crossing(uint seed, int2 border, int start, int size) {
    return start + seed_range(position_seed2(seed, border), size / 4, size * 3 / 4);
}

// a region's 4 crossings: west, east, south, north (region its RegionPosition, start its first block, size its blocks)
static inline void highway_crossings(uint seed, int2 region, int2 start, int2 size, HighwayEnd* ends) {
    const int2 end = { start.x + size.x - 1, start.y + size.y - 1 };
    ends[0] = (HighwayEnd) { { start.x, highway_crossing(seed, (int2) { region.x * 2, region.y * 2 + 1 }, start.y, size.y) }, { 1, 0 } };
    ends[1] = (HighwayEnd) { { end.x, highway_crossing(seed, (int2) { region.x * 2 + 2, region.y * 2 + 1 }, start.y, size.y) }, { -1, 0 } };
    ends[2] = (HighwayEnd) { { highway_crossing(seed, (int2) { region.x * 2 + 1, region.y * 2 }, start.x, size.x), start.y }, { 0, 1 } };
    ends[3] = (HighwayEnd) { { highway_crossing(seed, (int2) { region.x * 2 + 1, region.y * 2 + 2 }, start.x, size.x), end.y }, { 0, -1 } };
}

static inline int2 highway_clamp(int2 p, int2 start, int2 size) {
    return (int2) { int_clamp(p.x, start.x, start.x + size.x - 1), int_clamp(p.y, start.y, start.y + size.y - 1) };
}

static inline void highway_leg(Highways* h, int2 a, int2 b) {
    if (h->count < highways_max) {
        h->legs[h->count++] = (TownRoad) { a, b };
    }
}

// a highway from a to b: straight out of each, two legs between, kept in its region (start, size)
static inline void highway_join(Highways* h, HighwayEnd a, HighwayEnd b, int2 start, int2 size) {
    const int2 a2 = highway_clamp(int2_add(a.at, int2_multiply1(a.out, highway_out)), start, size);
    const int2 b2 = highway_clamp(int2_add(b.at, int2_multiply1(b.out, highway_out)), start, size);
    // across first along the way a leaves, if it leaves along x
    const int2 corner = a.out.x ? (int2) { b2.x, a2.y } : (int2) { a2.x, b2.y };
    highway_leg(h, a.at, a2);
    highway_leg(h, a2, corner);
    highway_leg(h, corner, b2);
    highway_leg(h, b2, b.at);
}

static inline int highway_distance(int2 a, int2 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// a region's highways, from its towns (each with its gate) and its borders
void region_highways(ecs* world, entity region, Highways* h) {
    h->count = 0;
    const entity terrain = zox_get_parent(world, region);
    const uint seed = zox_valid(terrain) && zox_has(terrain, Seed) ? (uint) zox_getv(terrain, Seed) : 0;
    const int2 start = zox_getv(region, BlockPosition2), size = zox_getv(region, BlockSize2);
    // its towns' gates, a block out of them
    HighwayEnd gates[highway_ends_max];
    byte gates_count = 0;
    entity towns[zox_children_capacity];
    const uint towns_count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    for (uint i = 0; i < towns_count && gates_count < highway_ends_max; i++) {
        const int2 out = build_turn(zox_getv(towns[i], TownGateSide), 0, 1);
        gates[gates_count++] = (HighwayEnd) { int2_add(town_gate_point(world, towns[i]), out), out };
    }
    // joined nearest first (Prim's): each round the nearest unjoined gate to a joined one
    byte joined[highway_ends_max] = { 1 };
    for (byte n = 1; n < gates_count; n++) {
        int best = -1, best_from = 0, best_distance = 0;
        for (byte i = 0; i < gates_count; i++) {
            for (byte j = 0; j < gates_count; j++) {
                if (!joined[i] || joined[j]) {
                    continue;
                }
                const int d = highway_distance(gates[i].at, gates[j].at);
                if (best < 0 || d < best_distance) {
                    best = j;
                    best_from = i;
                    best_distance = d;
                }
            }
        }
        joined[best] = 1;
        highway_join(h, gates[best_from], gates[best], start, size);
    }
    // its crossings, each to its nearest gate, or to its middle
    HighwayEnd crossings[4];
    highway_crossings(seed, zox_getv(region, RegionPosition), start, size, crossings);
    for (byte c = 0; c < 4; c++) {
        if (!gates_count) {
            const int2 middle = { start.x + size.x / 2, start.y + size.y / 2 };
            highway_join(h, crossings[c], (HighwayEnd) { middle, int2_multiply1(crossings[c].out, -1) }, start, size);
            continue;
        }
        byte nearest = 0;
        for (byte i = 1; i < gates_count; i++) {
            if (highway_distance(crossings[c].at, gates[i].at) < highway_distance(crossings[c].at, gates[nearest].at)) {
                nearest = i;
            }
        }
        highway_join(h, crossings[c], gates[nearest], start, size);
    }
}

// a leg's columns in a tunk: those whose cell (depth_difference blocks a side at lods) a highway crosses
static inline byte highway_leg_columns(const RegionTunk* t, const TownRoad* leg, int2* from, int2* to) {
    const int dd = t->depth_difference;
    const int2 low = { int_min(leg->a.x, leg->b.x) - highway_half - dd + 1, int_min(leg->a.y, leg->b.y) - highway_half - dd + 1 };
    const int2 high = { int_max(leg->a.x, leg->b.x) + highway_half, int_max(leg->a.y, leg->b.y) + highway_half };
    return region_tunk_range(t, low, high, from, to);
}
