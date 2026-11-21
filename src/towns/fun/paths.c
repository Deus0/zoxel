// Road segments: a road kept as straight segments, two points each (x, its ground, z), its ground -1 to keep the
//  ground it crosses (RoadSegments on a town; highways are worked out where they're drawn, fun/highways.c)
//  - drawn by town_map_system: a column within a road's half width of a segment is road, its ground set along the
//    segment's slope when it has one (cut or filled, the road level with its way)

// a path's points added as segments, pairs of points
static inline void road_segments_add(RoadSegments* segments, const int3* points, int count) {
    for (int k = 0; k + 1 < count; k++) {
        add_to_RoadSegments(segments, points[k]);
        add_to_RoadSegments(segments, points[k + 1]);
    }
}

// a column's nearness to a segment (a, b): its distance in blocks, t along it from a (0 to 1)
static inline float road_segment_distance(int3 a, int3 b, float2 p, float* t) {
    const float2 d = { b.x - a.x, b.z - a.z };
    const float length2 = d.x * d.x + d.y * d.y;
    *t = length2 > 0 ? clampf(((p.x - a.x) * d.x + (p.y - a.z) * d.y) / length2, 0, 1) : 0;
    const float x = a.x + d.x * *t - p.x, y = a.z + d.y * *t - p.y;
    return sqrtf(x * x + y * y);
}

// whether a column is on a road (within half of a segment), its ground there if so (-1 for its own)
static inline byte road_segments_at(const RoadSegments* segments, int2 g, float half, int* height) {
    for (int i = 0; i + 1 < segments->length; i += 2) {
        float t;
        if (road_segment_distance(segments->value[i], segments->value[i + 1], (float2) { g.x, g.y }, &t) <= half) {
            if (height) {
                const int3 a = segments->value[i], b = segments->value[i + 1];
                *height = a.y < 0 || b.y < 0 ? -1 : (int) roundf(a.y + (b.y - a.y) * t);
            }
            return 1;
        }
    }
    return 0;
}

// whether a segment (a to b, x and z) crosses a rect (its middle, half its size): a slab test
static inline byte segment_crosses_rect(float2 a, float2 b, int2 center, float2 half) {
    float t0 = 0, t1 = 1;
    const float d[2] = { b.x - a.x, b.y - a.y };
    const float p[2] = { a.x, a.y };
    const float low[2] = { center.x - half.x, center.y - half.y }, high[2] = { center.x + half.x, center.y + half.y };
    for (byte k = 0; k < 2; k++) {
        if (fabsf(d[k]) < 0.0001f) {
            if (p[k] < low[k] || p[k] > high[k]) {
                return 0;
            }
            continue;
        }
        float u0 = (low[k] - p[k]) / d[k], u1 = (high[k] - p[k]) / d[k];
        if (u0 > u1) {
            const float t = u0;
            u0 = u1;
            u1 = t;
        }
        t0 = u0 > t0 ? u0 : t0;
        t1 = u1 < t1 ? u1 : t1;
        if (t0 > t1) {
            return 0;
        }
    }
    return 1;
}
