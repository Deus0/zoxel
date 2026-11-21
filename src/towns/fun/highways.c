// Highways: a lattice of roads over the whole world, worked out where they're drawn (nothing holds them, no step)
//  - a node every highway_spacing blocks, jittered in its cell's middle half, from the terrain's seed
//  - each node links east or north from its seed (a tree that joins up as it goes), some both ways (loops)
//  - a node in or near a town or a mountain is left out, a link crossing one too (the heights a highway levels to
//    are the terrain's, mountains aren't in them: it cut trenches through them); each town links its gate (straight
//    out highway_out blocks first) to its nearest node left with a clear line
//  - a tunk draws the links crossing it (highways_in_box): only the nodes round it, their regions' towns and two
//    heights a link (the terrain's at its ends, water's top over water: a causeway), the same whichever tunk draws it
//  - across regions and their borders as if they weren't there: no crossings to agree on
#define highway_half 2          // 5 blocks wide
#define highway_out 12
#define highway_spacing 128
#define highway_loops 3         // in 8 nodes link both ways
#define highway_town_margin 16  // a node this near a town's walls is left out
#define highway_towns_max 16    // a region's
#define highway_regions_max 16

typedef struct {
    int2 center;
    byte2 size;
    int2 gate;
    byte gate_side;
    int2 out;           // straight out of its gate, highway_out blocks past its rect's edge (its wall curves inside it)
} HighwayTown;

typedef struct {
    int2 position;
    HighwayTown towns[highway_towns_max];
    byte count;
    int2 mountains[highway_towns_max];      // its placement circles: mountains, dungeons
    byte mountain_radii[highway_towns_max];
    byte mountains_count;
} HighwayRegion;

typedef struct {
    ecs* world;
    entity terrain;
    uint seed;
    byte terrain_depth;
    HighwayRegion regions[highway_regions_max];
    byte regions_count;
    byte waiting;           // a region round it is still placing its towns: draw later
    int2 home;              // the region it's drawn for: its ring is spawned with it, waited on if missing
} Highways;

static inline void highways_begin(Highways* h, ecs* world, entity terrain, int2 block) {
    h->world = world;
    h->terrain = terrain;
    h->seed = (uint) zox_getv(terrain, Seed);
    h->terrain_depth = zox_getv(terrain, NodeDepth);
    h->regions_count = 0;
    h->waiting = 0;
    h->home = block_position_to_region_position(block, h->terrain_depth);
}

// the region a block is in, its towns (cached), NULL if it has none yet
static inline const HighwayRegion* highway_region(Highways* h, int2 block) {
    ecs* world = h->world;
    const int2 position = block_position_to_region_position(block, h->terrain_depth);
    for (byte i = 0; i < h->regions_count; i++) {
        if (int2_equals(h->regions[i].position, position)) {
            return &h->regions[i];
        }
    }
    if (h->regions_count >= highway_regions_max) {
        return NULL;
    }
    const entity region = int2_hashmap_get(zox_get(h->terrain, RegionLinks)->value, position);
    if (!zox_valid(region)) {
        // the stream spawns the regions round every region it streams (terrain/sys/stream.c): one of those is coming,
        //  one farther may never (a town in the ring looks 2 cells past it): none of its towns then
        if (abs(position.x - h->home.x) <= 1 && abs(position.y - h->home.y) <= 1) {
            h->waiting = 1;
        }
        return NULL;
    }
    const byte generate = zox_getv(region, GenerateRegion);
    if (generate >= zox_generate_region_mountains && generate <= zox_generate_region_towns) {
        h->waiting = 1;
        return NULL;
    }
    HighwayRegion* r = &h->regions[h->regions_count++];
    r->position = position;
    r->count = 0;
    r->mountains_count = 0;
    entity zones[zox_children_capacity];
    const uint zones_count = zox_get_children_by_id(world, region, zones, zox_children_capacity, zox_id(PlacementZone));
    for (uint i = 0; i < zones_count && r->mountains_count < highway_towns_max; i++) {
        if (zox_has(zones[i], Radius)) {
            r->mountains[r->mountains_count] = zox_getv(zones[i], BlockPosition2);
            r->mountain_radii[r->mountains_count++] = zox_getv(zones[i], Radius);
        }
    }
    entity towns[zox_children_capacity];
    const uint count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    for (uint i = 0; i < count && r->count < highway_towns_max; i++) {
        const TownShape shape = town_shape_of(world, towns[i]);
        const int rect_out = (shape.gate_side & 1 ? zox_getv(towns[i], TownSize).x : zox_getv(towns[i], TownSize).y) / 2;
        r->towns[r->count++] = (HighwayTown) { shape.center, zox_getv(towns[i], TownSize), shape.gate, shape.gate_side,
            int2_add(shape.center, build_turn(shape.gate_side, shape.gate_along, rect_out + highway_out)) };
    }
    return r;
}

// a town or mountain near a point (within margin), 0 if none
static inline byte highway_blocked_near(Highways* h, int2 p, int margin) {
    const HighwayRegion* r = highway_region(h, p);
    for (byte i = 0; r && i < r->count; i++) {
        const HighwayTown* t = &r->towns[i];
        if (abs(p.x - t->center.x) <= t->size.x / 2 + margin && abs(p.y - t->center.y) <= t->size.y / 2 + margin) {
            return 1;
        }
    }
    for (byte i = 0; r && i < r->mountains_count; i++) {
        const int d = r->mountain_radii[i] + margin;
        if ((p.x - r->mountains[i].x) * (p.x - r->mountains[i].x) + (p.y - r->mountains[i].y) * (p.y - r->mountains[i].y) <= d * d) {
            return 1;
        }
    }
    return 0;
}

// a lattice cell's node, jittered in its middle half
static inline int2 highway_node_at(uint seed, int2 cell) {
    const uint a = position_seed2(seed + 7741, cell), b = position_seed2(seed + 4219, cell);
    return (int2) { cell.x * highway_spacing + highway_spacing / 4 + (int) (a % (highway_spacing / 2)),
        cell.y * highway_spacing + highway_spacing / 4 + (int) (b % (highway_spacing / 2)) };
}

// a lattice cell's node, 0 if it's left out (in or near a town)
static inline byte highway_node(Highways* h, int2 cell, int2* node) {
    *node = highway_node_at(h->seed, cell);
    return !highway_blocked_near(h, *node, highway_town_margin);
}

// a node's links: 1 east, 2 north, 3 both
static inline byte highway_links(uint seed, int2 cell) {
    const uint a = position_seed2(seed + 9151, cell);
    return (a >> 1) % 8 < highway_loops ? 3 : (a & 1) ? 1 : 2;
}

// a straight highway from a to b misses the towns and mountains (unless over_mountains) of both ends' regions (a link
//  crosses one border at most)
static inline byte highway_clear_over(Highways* h, int2 a, int2 b, byte over_mountains) {
    const HighwayRegion* regions[2] = { highway_region(h, a), highway_region(h, b) };
    for (byte k = 0; k < 2; k++) {
        for (byte i = 0; regions[k] && i < regions[k]->count; i++) {
            const HighwayTown* t = &regions[k]->towns[i];
            const float2 half = { t->size.x / 2 + highway_half + 1, t->size.y / 2 + highway_half + 1 };
            if (segment_crosses_rect((float2) { a.x, a.y }, (float2) { b.x, b.y }, t->center, half)) {
                return 0;
            }
        }
        for (byte i = 0; !over_mountains && regions[k] && i < regions[k]->mountains_count; i++) {
            float t;
            const int2 m = regions[k]->mountains[i];
            if (road_segment_distance((int3) { a.x, 0, a.y }, (int3) { b.x, 0, b.y }, (float2) { m.x, m.y }, &t) <=
                regions[k]->mountain_radii[i] + highway_half) {
                return 0;
            }
        }
    }
    return 1;
}

static inline byte highway_clear(Highways* h, int2 a, int2 b) {
    return highway_clear_over(h, a, b, 0);
}

// a segment's box crosses low to high (blocks), its half width round it
static inline byte highway_in_box(int2 a, int2 b, int2 low, int2 high) {
    return int_max(a.x, b.x) + highway_half >= low.x && int_min(a.x, b.x) - highway_half <= high.x &&
        int_max(a.y, b.y) + highway_half >= low.y && int_min(a.y, b.y) - highway_half <= high.y;
}

static inline void highway_add(int2* ends, int* count, int max, int2 a, int2 b) {
    if (*count + 2 <= max) {
        ends[(*count)++] = a;
        ends[(*count)++] = b;
    }
}

// a town's link: its gate, straight out, then to its nearest node left with a clear line, 2 cells round; over a
//  mountain if no line misses them (a town with no highway was worse), 0 if none
static inline byte highway_town_link(Highways* h, const HighwayTown* t, int2* out, int2* node) {
    *out = t->out;
    const int2 cell = { floor_div(out->x, highway_spacing), floor_div(out->y, highway_spacing) };
    int best = -1;
    for (byte over = 0; over < 2 && best < 0; over++)
    for (int y = -2; y <= 2; y++)
    for (int x = -2; x <= 2; x++) {
        int2 n;
        if (!highway_node(h, (int2) { cell.x + x, cell.y + y }, &n) || !highway_clear_over(h, *out, n, over)) {
            continue;
        }
        const int d = abs(n.x - out->x) + abs(n.y - out->y);
        if (best < 0 || d < best) {
            best = d;
            *node = n;
        }
    }
    return best >= 0;
}

// the highways crossing a box of blocks (low to high): their ends as pairs (x, z), their count of ends
int highways_in_box(Highways* h, int2 low, int2 high, int2* ends, int max) {
    int count = 0;
    // the links of the nodes round it
    const int2 from = { floor_div(low.x, highway_spacing) - 1, floor_div(low.y, highway_spacing) - 1 };
    const int2 to = { floor_div(high.x, highway_spacing) + 1, floor_div(high.y, highway_spacing) + 1 };
    for (int y = from.y; y <= to.y; y++)
    for (int x = from.x; x <= to.x; x++) {
        const int2 cell = { x, y };
        const byte links = highway_links(h->seed, cell);
        for (byte k = 0; k < 2; k++) {
            if (!(links & (1 << k))) {
                continue;
            }
            const int2 next = { x + !k, y + k };
            int2 a = highway_node_at(h->seed, cell), b = highway_node_at(h->seed, next);
            // its box first: most links are far from it, no towns to look up for those
            if (!highway_in_box(a, b, low, high) || !highway_node(h, cell, &a) || !highway_node(h, next, &b) ||
                !highway_clear(h, a, b)) {
                continue;
            }
            highway_add(ends, &count, max, a, b);
        }
    }
    // the towns near it, their gates to their nodes (a town's link is under 3 lattice cells long)
    const int reach = highway_spacing * 3;
    const int region_blocks = octree_size(h->terrain_depth) * region_dividor;
    for (int y = floor_div(low.y - reach, region_blocks); y <= floor_div(high.y + reach, region_blocks); y++)
    for (int x = floor_div(low.x - reach, region_blocks); x <= floor_div(high.x + reach, region_blocks); x++) {
        highway_region(h, (int2) { x * region_blocks, y * region_blocks });
    }
    for (byte r = 0; r < h->regions_count; r++) {
        for (byte i = 0; i < h->regions[r].count; i++) {
            const HighwayTown* t = &h->regions[r].towns[i];
            int2 out, node;
            if (!highway_in_box(t->gate, t->gate, int2_sub(low, int2_single(reach)), int2_add(high, int2_single(reach))) ||
                !highway_town_link(h, t, &out, &node)) {
                continue;
            }
            const int2 gate_out = int2_add(t->gate, build_turn(t->gate_side, 0, 1));
            if (highway_in_box(gate_out, out, low, high)) {
                highway_add(ends, &count, max, gate_out, out);
            }
            if (highway_in_box(out, node, low, high)) {
                highway_add(ends, &count, max, out, node);
            }
        }
    }
    return count;
}

// a highway's ground at a point: the terrain's, or water's top (a causeway)
static inline int highway_ground(const TerrainHeights* th, int sea, int2 at) {
    return int_max(terrain_height_at(th, at), terrain_water_level_at(th, at, sea));
}

// a segment's columns in a tunk: those whose cell (depth_difference blocks a side at lods) it may cross, half wide
static inline byte road_segment_columns(const RegionTunk* t, int3 a, int3 b, int half, int2* from, int2* to) {
    const int dd = t->depth_difference;
    const int2 low = { int_min(a.x, b.x) - half - dd + 1, int_min(a.z, b.z) - half - dd + 1 };
    const int2 high = { int_max(a.x, b.x) + half, int_max(a.z, b.z) + half };
    return region_tunk_range(t, low, high, from, to);
}
