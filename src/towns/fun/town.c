// A town's gate, square and roads (town_map_system marks them, town_walls_system paves them)
//  - its gate: in one wall (TownGateSide) at a spot along it (TownGateAt), from its seed
//  - its square: paved in its middle, its homes placed round it (homes_spawn_system)
//  - its roads: from each home's door to the square, and from the square out to the gate, each two straight legs
//    (out from the door or gate first, then across), a few blocks wide
#define town_gate_width 4
#define town_square_half 4
#define town_road_half 1
#define town_roads_max (town_homes_max * 2 + 2)

typedef struct {
    int2 a;
    int2 b;         // a straight leg, along x or z
} TownRoad;

typedef struct {
    int2 square;
    TownRoad roads[town_roads_max];
    byte count;
} TownRoads;

// its gate's middle, on its wall's outer edge
static inline int2 town_gate_point(ecs* world, entity town) {
    const int2 center = zox_getv(town, BlockPosition2);
    const byte2 size = zox_getv(town, TownSize);
    const byte side = zox_getv(town, TownGateSide);
    const int along = (int) zox_getv(town, TownGateAt) - 128;
    const int out = (side & 1 ? size.x : size.y) / 2;
    return int2_add(center, build_turn(side, along, out));
}

// a road from a to b: out along the way it leaves first (along z if z_first), then across
static inline void town_road(TownRoads* r, int2 a, int2 b, byte z_first) {
    if (r->count + 2 > town_roads_max) {
        return;
    }
    const int2 corner = z_first ? (int2) { a.x, b.y } : (int2) { b.x, a.y };
    r->roads[r->count++] = (TownRoad) { a, corner };
    r->roads[r->count++] = (TownRoad) { corner, b };
}

// a town's square and its roads: to each home's door, out to its gate
static inline void town_roads(ecs* world, entity town, TownRoads* r) {
    r->square = zox_getv(town, BlockPosition2);
    r->count = 0;
    const byte side = zox_getv(town, TownGateSide);
    town_road(r, town_gate_point(world, town), r->square, !(side & 1));
    entity homes[zox_children_capacity];
    const uint homes_count = zox_get_children_by_id(world, town, homes, zox_children_capacity, zox_id(Home));
    for (uint i = 0; i < homes_count; i++) {
        const TownHome h = town_home(world, homes[i]);
        const int2 door = int2_add(h.center, build_turn(h.facing, 0, home_half_local(&h).y + 1));
        town_road(r, door, r->square, !(h.facing & 1));
    }
}

// a column on its square or a road
static inline byte town_is_road(const TownRoads* r, int2 g) {
    if (abs(g.x - r->square.x) <= town_square_half && abs(g.y - r->square.y) <= town_square_half) {
        return 1;
    }
    for (byte i = 0; i < r->count; i++) {
        const TownRoad* road = &r->roads[i];
        if (g.x >= int_min(road->a.x, road->b.x) - town_road_half && g.x <= int_max(road->a.x, road->b.x) + town_road_half &&
            g.y >= int_min(road->a.y, road->b.y) - town_road_half && g.y <= int_max(road->a.y, road->b.y) + town_road_half) {
            return 1;
        }
    }
    return 0;
}

// a column in its gate: its wall band on the gate's side, beside the gate's middle
static inline byte town_is_gate(ecs* world, entity town, int2 g) {
    const int2 gate = town_gate_point(world, town);
    const byte side = zox_getv(town, TownGateSide);
    const byte thickness = zox_getv(town, WallThickness);
    const int2 local = build_unturn(side, int2_sub(g, gate));
    return abs(local.x) <= town_gate_width / 2 && local.y <= 0 && local.y > -thickness;
}

// the town whose walls block (global, x and z) is inside, 0 for none
static inline entity town_at(ecs* world, int2 block) {
    ecs_iter_t it = ecs_each_id(world, zox_id(Town));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity town = it.entities[i];
            const int2 center = zox_getv(town, BlockPosition2);
            const byte2 size = zox_getv(town, TownSize);
            if (size.x && abs(block.x - center.x) * 2 < size.x && abs(block.y - center.y) * 2 < size.y) {
                ecs_iter_fini(&it);
                return town;
            }
        }
    }
    return 0;
}
