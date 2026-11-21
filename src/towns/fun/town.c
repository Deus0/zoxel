// A town's gate, square, roads and street lamps (town_map_system marks them, town_walls_system paves them)
//  - its gate: in one wall (TownGateSide) at a spot along it (TownGateAt), from its seed (town_gate_place)
//  - its square: paved in its middle, its homes placed round it (homes_spawn_system)
//  - its roads (town_roads_build, once its homes are placed): a waypoint graph inside its walls (a point jittered every
//    town_waypoint_spacing blocks, none in homes, and each home's corners a block past its margin, so a way round a
//    home is always there), linked where a straight line misses every home; the shortest ways
//    from its square to its gate and each door (one tree, so roads share their way), straightened; kept on it
//    (RoadSegments), on the ground as it is (homes level their own)
//  - its street lamps: at its square's corners, and beside its roads every town_lamp_spacing blocks along them, off
//    roads and homes, apart from each other (StreetLamps)
#define town_lamp_spacing 16
#define town_lamp_apart 8           // blocks between lamps at least
#define town_lamps_max 48
#define town_gate_width 4
#define town_square_half 4
#define town_road_half 1
#define town_waypoint_spacing 8
#define town_waypoint_reach 20      // blocks a waypoint links to others within
#define town_waypoints_max 512

// a town's gate from its seed: its wall (0 +z, 1 +x, 2 -z, 3 -x) and its spot along it (blocks from its middle,
//  clear of its corners)
static inline void town_gate_place(lint seed, byte2 size, byte wall_thickness, byte* side, int* along) {
    *side = seed_range(seed * 31, 0, 4);
    const int wall_half = (*side & 1 ? size.y : size.x) / 2 - wall_thickness - 4;
    *along = wall_half > 0 ? seed_range(seed * 37, -wall_half, wall_half + 1) : 0;
}

// its gate's middle, on its wall's outer edge
static inline int2 town_gate_at(int2 center, byte2 size, byte side, int along) {
    const int out = (side & 1 ? size.x : size.y) / 2;
    return int2_add(center, build_turn(side, along, out));
}

static inline int2 town_gate_point(ecs* world, entity town) {
    return town_gate_at(zox_getv(town, BlockPosition2), zox_getv(town, TownSize), zox_getv(town, TownGateSide),
        (int) zox_getv(town, TownGateAt) - 128);
}

// a column on its square
static inline byte town_is_square(int2 center, int2 g) {
    return abs(g.x - center.x) <= town_square_half && abs(g.y - center.y) <= town_square_half;
}

// a column in a home or its margin
static inline byte town_in_home(const TownHome* homes, byte count, int2 g, int margin) {
    for (byte i = 0; i < count; i++) {
        if (abs(g.x - homes[i].center.x) <= homes[i].half.x + margin && abs(g.y - homes[i].center.y) <= homes[i].half.y + margin) {
            return 1;
        }
    }
    return 0;
}

// a street lamp at g if it's clear: inside its walls, off its roads, square and homes, apart from the others
static inline void town_lamp_try(StreetLamps* lamps, const RoadSegments* roads, const TownHome* homes, byte homes_count,
    int2 center, int2 inner_low, int2 inner_high, int2 g)
{
    if (lamps->length >= town_lamps_max || g.x < inner_low.x || g.y < inner_low.y || g.x > inner_high.x ||
        g.y > inner_high.y || town_is_square(center, g) || town_in_home(homes, homes_count, g, 1) ||
        road_segments_at(roads, g, town_road_half + 0.5f, NULL)) {
        return;
    }
    for (int i = 0; i < lamps->length; i++) {
        if (abs(lamps->value[i].x - g.x) < town_lamp_apart && abs(lamps->value[i].y - g.y) < town_lamp_apart) {
            return;
        }
    }
    add_to_StreetLamps(lamps, g);
}

// a straight road from a to b misses every home (grown by half a block: a road's edge may touch a wall, not cross it)
static inline byte town_road_clear(float2 a, float2 b, const TownHome* homes, byte homes_count) {
    for (byte i = 0; i < homes_count; i++) {
        if (segment_crosses_rect(a, b, homes[i].center, (float2) { homes[i].half.x + 0.5f, homes[i].half.y + 0.5f })) {
            return 0;
        }
    }
    return 1;
}

// a town's roads and street lamps, once its homes are placed
void town_roads_build(uint seed, int2 center, byte2 size, byte wall_thickness, byte gate_side, int2 gate,
    const TownHome* homes, byte homes_count, RoadSegments* roads, StreetLamps* lamps)
{
    resize_RoadSegments(roads, 0);
    resize_StreetLamps(lamps, 0);
    const int2 inner_low = { center.x - size.x / 2 + wall_thickness, center.y - size.y / 2 + wall_thickness };
    const int2 inner_high = { center.x + size.x / 2 - wall_thickness, center.y + size.y / 2 - wall_thickness };
    // its waypoints: the square's middle, inside its gate, each door, then jittered ones off its homes
    float2 points[town_waypoints_max];
    int count = 0;
    const int2 inside = int2_add(gate, build_turn(gate_side, 0, -wall_thickness));
    points[count++] = (float2) { center.x, center.y };
    points[count++] = (float2) { inside.x, inside.y };
    const int doors_start = count;
    for (byte i = 0; i < homes_count && count < town_waypoints_max; i++) {
        const int2 door = int2_add(homes[i].center, build_turn(homes[i].facing, 0, home_half_local(&homes[i]).y + 1));
        points[count++] = (float2) { door.x, door.y };
    }
    // each home's corners, round it
    for (byte i = 0; i < homes_count; i++) {
        for (byte c = 0; c < 4 && count < town_waypoints_max; c++) {
            const int2 p = { homes[i].center.x + (c & 1 ? 1 : -1) * (homes[i].half.x + 2),
                homes[i].center.y + (c & 2 ? 1 : -1) * (homes[i].half.y + 2) };
            if (p.x >= inner_low.x && p.y >= inner_low.y && p.x <= inner_high.x && p.y <= inner_high.y &&
                !town_in_home(homes, homes_count, p, 1)) {
                points[count++] = (float2) { p.x, p.y };
            }
        }
    }
    for (int z = inner_low.y + 2; z <= inner_high.y - 2 && count < town_waypoints_max; z += town_waypoint_spacing)
    for (int x = inner_low.x + 2; x <= inner_high.x - 2 && count < town_waypoints_max; x += town_waypoint_spacing) {
        const uint h = position_seed2(seed, (int2) { x, z });
        const int2 p = { x + (int) (h % 5) - 2, z + (int) ((h / 5) % 5) - 2 };
        if (!town_is_square(center, p) && !town_in_home(homes, homes_count, p, 2)) {
            points[count++] = (float2) { p.x, p.y };
        }
    }
    // the shortest ways from the square (Dijkstra over waypoints linked within reach by clear lines)
    float distance[town_waypoints_max];
    short from[town_waypoints_max];
    byte done[town_waypoints_max];
    for (int i = 0; i < count; i++) {
        distance[i] = 1e9f;
        from[i] = -1;
        done[i] = 0;
    }
    distance[0] = 0;
    for (int n = 0; n < count; n++) {
        int u = -1;
        for (int i = 0; i < count; i++) {
            if (!done[i] && distance[i] < 1e8f && (u < 0 || distance[i] < distance[u])) {
                u = i;
            }
        }
        if (u < 0) {
            break;
        }
        done[u] = 1;
        for (int v = 0; v < count; v++) {
            if (done[v]) {
                continue;
            }
            const float dx = points[v].x - points[u].x, dz = points[v].y - points[u].y;
            const float d2 = dx * dx + dz * dz;
            if (d2 > town_waypoint_reach * town_waypoint_reach || distance[u] + sqrtf(d2) >= distance[v] ||
                !town_road_clear(points[u], points[v], homes, homes_count)) {
                continue;
            }
            distance[v] = distance[u] + sqrtf(d2);
            from[v] = (short) u;
        }
    }
    // each door's way and the gate's, back to the square, straightened (the farthest point it sees clearly)
    for (int target = 1; target < doors_start + homes_count && target < count; target++) {
        if (from[target] < 0) {
            continue;
        }
        float2 way[town_waypoints_max];
        int length = 0;
        for (int i = target; i >= 0 && length < town_waypoints_max; i = from[i]) {
            way[length++] = points[i];
        }
        int3 straight[town_waypoints_max];
        int straight_count = 0;
        straight[straight_count++] = (int3) { (int) way[0].x, -1, (int) way[0].y };
        for (int k = 0; k < length - 1;) {
            int far = k + 1;
            for (int m = length - 1; m > k + 1; m--) {
                if (town_road_clear(way[k], way[m], homes, homes_count)) {
                    far = m;
                    break;
                }
            }
            straight[straight_count++] = (int3) { (int) way[far].x, -1, (int) way[far].y };
            k = far;
        }
        road_segments_add(roads, straight, straight_count);
    }
    // through its gate
    const int3 through[2] = { { gate.x, -1, gate.y }, { inside.x, -1, inside.y } };
    road_segments_add(roads, through, 2);
    // its lamps: the square's corners, then along its roads
    const int corner = town_square_half + 1;
    for (byte c = 0; c < 4; c++) {
        town_lamp_try(lamps, roads, homes, homes_count, center, inner_low, inner_high,
            (int2) { center.x + (c & 1 ? corner : -corner), center.y + (c & 2 ? corner : -corner) });
    }
    for (int i = 0; i + 1 < roads->length; i += 2) {
        const int3 a = roads->value[i], b = roads->value[i + 1];
        const float2 d = { b.x - a.x, b.z - a.z };
        const float length = sqrtf(d.x * d.x + d.y * d.y);
        if (length < 1) {
            continue;
        }
        // beside it, left of its way, a block past its edge
        const float2 side = { -d.y / length * (town_road_half + 1.5f), d.x / length * (town_road_half + 1.5f) };
        for (float along = town_lamp_spacing / 2; along < length; along += town_lamp_spacing) {
            const int2 g = { (int) roundf(a.x + d.x / length * along + side.x), (int) roundf(a.z + d.y / length * along + side.y) };
            town_lamp_try(lamps, roads, homes, homes_count, center, inner_low, inner_high, g);
        }
    }
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
