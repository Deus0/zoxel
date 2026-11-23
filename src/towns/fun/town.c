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

// A town's wall: a closed curve inside its rect (TownSize), not a box. A rounded rect (a superellipse: its power from
//  its seed, rounder low, squarer high), pulled in round its length by a little smooth noise (three waves, their
//  phases from its seed), its band WallThickness blocks in from it
//  - worked out from its seed, size and thickness wherever it's needed (nothing stored): the map (town_wall_value),
//    its gate, homes kept inside it, its roads' waypoints and lamps
//  - its gate: along its side (TownGateAt) as before, out to where that line meets the curve
#define town_wall_jitter 0.1f       // the noise pulls the curve in by up to a tenth
#define town_wall_spikes 3          // blocks round its edges between spikes

typedef struct {
    int2 center;
    float2 half;
    float power;
    float phases[3];
    byte thickness;
    byte gate_side;
    int gate_along;
    int2 gate;          // its gate's middle on the wall's outer edge
} TownShape;

// its outer edge's distance from its middle, in blocks, along an angle
static inline float town_shape_radius(const TownShape* s, float angle) {
    const float c = fabsf(cosf(angle)) / s->half.x, n = fabsf(sinf(angle)) / s->half.y;
    const float r = powf(powf(c, s->power) + powf(n, s->power), -1.0f / s->power);
    const float noise = 0.5f * sinf(angle * 3 + s->phases[0]) + 0.3f * sinf(angle * 5 + s->phases[1]) +
        0.2f * sinf(angle * 7 + s->phases[2]);
    return r * (1 - town_wall_jitter * (0.5f + 0.5f * noise));
}

// how far a column is in from the wall's outer edge, in blocks (negative outside it)
static inline float town_shape_depth(const TownShape* s, int2 g) {
    const float dx = g.x - s->center.x, dz = g.y - s->center.y;
    const float d = sqrtf(dx * dx + dz * dz);
    return d < 0.001f ? s->half.x : town_shape_radius(s, atan2f(dz, dx)) - d;
}

// a column inside its walls, margin blocks clear of them
static inline byte town_inside(const TownShape* s, int2 g, int margin) {
    return town_shape_depth(s, g) >= s->thickness + margin;
}

static inline TownShape town_shape(lint seed, int2 center, byte2 size, byte thickness, byte gate_side, int gate_along) {
    TownShape s = { center, { size.x / 2.0f, size.y / 2.0f }, 2.5f + seed_range(seed * 41, 0, 250) / 100.0f,
        { seed_range(seed * 43, 0, 628) / 100.0f, seed_range(seed * 47, 0, 628) / 100.0f, seed_range(seed * 53, 0, 628) / 100.0f },
        thickness, gate_side, gate_along, center };
    // its gate: out along its side's line to the curve's outer edge
    const int most = (gate_side & 1 ? size.x : size.y) / 2;
    for (int out = 0; out <= most; out++) {
        const int2 g = int2_add(center, build_turn(gate_side, gate_along, out));
        if (town_shape_depth(&s, g) < 0) {
            break;
        }
        s.gate = g;
    }
    return s;
}

static inline TownShape town_shape_of(ecs* world, entity town) {
    return town_shape(zox_getv(town, Seed), zox_getv(town, BlockPosition2), zox_getv(town, TownSize),
        zox_getv(town, WallThickness), zox_getv(town, TownGateSide), (int) zox_getv(town, TownGateAt) - 128);
}

// its gate's middle, on its wall's outer edge
static inline int2 town_gate_point(ecs* world, entity town) {
    return town_shape_of(world, town).gate;
}

// a column of its wall's band in its gate: beside the gate's line, in from its outer edge (a slanted band too)
static inline byte town_is_gate(const TownShape* s, int2 g) {
    const int2 local = build_unturn(s->gate_side, int2_sub(g, s->gate));
    return abs(local.x) <= town_gate_width / 2 && local.y <= 0 && local.y > -(s->thickness * 2 + 2);
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
    const TownShape* shape, int2 g)
{
    if (lamps->length >= town_lamps_max || !town_inside(shape, g, 1) || town_is_square(shape->center, g) ||
        town_in_home(homes, homes_count, g, 1) ||
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
//  and stays inside the town's walls (they curve in: a line between two points inside can leave)
static inline byte town_road_clear(float2 a, float2 b, const TownHome* homes, byte homes_count, const TownShape* shape) {
    for (byte i = 0; i < homes_count; i++) {
        if (segment_crosses_rect(a, b, homes[i].center, (float2) { homes[i].half.x + 0.5f, homes[i].half.y + 0.5f })) {
            return 0;
        }
    }
    const float length = sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    for (float t = 0; t <= length; t += 2) {
        const float f = length > 0 ? t / length : 0;
        if (!town_inside(shape, (int2) { (int) roundf(a.x + (b.x - a.x) * f), (int) roundf(a.y + (b.y - a.y) * f) }, 0)) {
            return 0;
        }
    }
    return 1;
}

// a town's roads and street lamps, once its homes are placed
void town_roads_build(uint seed, const TownShape* shape, const TownHome* homes, byte homes_count, RoadSegments* roads,
    StreetLamps* lamps)
{
    resize_RoadSegments(roads, 0);
    resize_StreetLamps(lamps, 0);
    const int2 center = shape->center, gate = shape->gate;
    const int2 low = { center.x - (int) shape->half.x, center.y - (int) shape->half.y };
    const int2 high = { center.x + (int) shape->half.x, center.y + (int) shape->half.y };
    // its waypoints: the square's middle, inside its gate, each door, then jittered ones off its homes
    float2 points[town_waypoints_max];
    int count = 0;
    // inside its gate: in from it along its line, past the wall's band
    int2 inside = gate;
    for (int in = 0; in < shape->thickness * 3 + 4 && !town_inside(shape, inside, 1); in++) {
        inside = int2_add(gate, build_turn(shape->gate_side, 0, -in));
    }
    points[count++] = (float2) { center.x, center.y };
    points[count++] = (float2) { inside.x, inside.y };
    const int doors_start = count;
    for (byte i = 0; i < homes_count && count < town_waypoints_max; i++) {
        const int2 door = int2_add(homes[i].center, build_turn(homes[i].facing, 0, home_half_local(&homes[i]).y + 1));
        points[count++] = (float2) { door.x, door.y };
    }
    // beside each door, along its front a block past its sides: a door with a home close in front of it saw no other
    //  waypoint clearly (its corners' ones in the other's margin) and got no road (tst town_homes, 1 world in 5)
    for (byte i = 0; i < homes_count; i++) {
        const int out = home_half_local(&homes[i]).y + 1, across = home_half_local(&homes[i]).x + 1;
        for (int side = -1; side <= 1 && count < town_waypoints_max; side += 2) {
            const int2 p = int2_add(homes[i].center, build_turn(homes[i].facing, side * across, out));
            if (town_inside(shape, p, 1) && !town_in_home(homes, homes_count, p, 0)) {
                points[count++] = (float2) { p.x, p.y };
            }
        }
    }
    // each home's corners, round it
    for (byte i = 0; i < homes_count; i++) {
        for (byte c = 0; c < 4 && count < town_waypoints_max; c++) {
            const int2 p = { homes[i].center.x + (c & 1 ? 1 : -1) * (homes[i].half.x + 2),
                homes[i].center.y + (c & 2 ? 1 : -1) * (homes[i].half.y + 2) };
            if (town_inside(shape, p, 1) && !town_in_home(homes, homes_count, p, 1)) {
                points[count++] = (float2) { p.x, p.y };
            }
        }
    }
    for (int z = low.y + 2; z <= high.y - 2 && count < town_waypoints_max; z += town_waypoint_spacing)
    for (int x = low.x + 2; x <= high.x - 2 && count < town_waypoints_max; x += town_waypoint_spacing) {
        const uint h = position_seed2(seed, (int2) { x, z });
        const int2 p = { x + (int) (h % 5) - 2, z + (int) ((h / 5) % 5) - 2 };
        if (town_inside(shape, p, 2) && !town_is_square(center, p) && !town_in_home(homes, homes_count, p, 2)) {
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
                !town_road_clear(points[u], points[v], homes, homes_count, shape)) {
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
            int farthest = k + 1;
            for (int m = length - 1; m > k + 1; m--) {
                if (town_road_clear(way[k], way[m], homes, homes_count, shape)) {
                    farthest = m;
                    break;
                }
            }
            straight[straight_count++] = (int3) { (int) way[farthest].x, -1, (int) way[farthest].y };
            k = farthest;
        }
        road_segments_add(roads, straight, straight_count);
    }
    // through its gate
    const int3 through[2] = { { gate.x, -1, gate.y }, { inside.x, -1, inside.y } };
    road_segments_add(roads, through, 2);
    // its lamps: the square's corners, then along its roads
    const int corner = town_square_half + 1;
    for (byte c = 0; c < 4; c++) {
        town_lamp_try(lamps, roads, homes, homes_count, shape,
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
            town_lamp_try(lamps, roads, homes, homes_count, shape, g);
        }
    }
}

// a column's town map value in its rect: nothing outside its wall, its wall (edges a block higher, spikes along them,
//  its gate a gap), its square, the rest its ground (roads, lamps and homes are drawn on it after)
static inline byte town_wall_value(const TownShape* s, int2 g) {
    const float depth = town_shape_depth(s, g);
    if (depth < 0) {
        return zox_town_type_none;
    }
    if (depth >= s->thickness) {
        return town_is_square(s->center, g) ? zox_town_type_road : zox_town_type_zone;
    }
    if (town_is_gate(s, g)) {
        return zox_town_type_gate;
    }
    if (depth >= 1 && depth < s->thickness - 1) {
        return zox_town_type_wall;
    }
    // its edges, a spike every few blocks round them
    const float dx = g.x - s->center.x, dz = g.y - s->center.y;
    const int along = (int) floorf((atan2f(dz, dx) + (float) M_PI) * sqrtf(dx * dx + dz * dz));
    return along % town_wall_spikes == 0 ? zox_town_type_wall_spike : zox_town_type_wall_edge;
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
