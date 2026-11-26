typedef struct {
    float3 points[3];
    float3 normal;
} poly_triangle;

typedef struct {
    float3 segment;
    float3 triangle;
    float distance_squared;
} poly_closest_pair;

static poly_triangle poly_triangle_new(float3 a, float3 b, float3 c) {
    return (poly_triangle) { { a, b, c }, float3_normalize(float3_cross(float3_sub(b, a), float3_sub(c, a))) };
}

static byte poly_triangle_contains(poly_triangle triangle, float3 point) {
    for (int i = 0; i < 3; i++) {
        const float3 a = triangle.points[i];
        const float3 edge = float3_sub(triangle.points[(i + 1) % 3], a);
        if (float3_dot(float3_cross(edge, float3_sub(point, a)), triangle.normal) < 0) {
            return 0;
        }
    }
    return 1;
}

static float3 poly_closest_on_edge(float3 point, float3 a, float3 b) {
    const float3 edge = float3_sub(b, a);
    const float length_squared = float3_dot(edge, edge);
    const float t = length_squared > 0 ? clampf(float3_dot(float3_sub(point, a), edge) / length_squared, 0, 1) : 0;
    return float3_add(a, float3_scale(edge, t));
}

static void poly_keep_closest(poly_closest_pair* best, float3 segment, float3 triangle) {
    const float3 delta = float3_sub(segment, triangle);
    const float distance_squared = float3_dot(delta, delta);
    if (distance_squared < best->distance_squared) {
        *best = (poly_closest_pair) { segment, triangle, distance_squared };
    }
}

static float3 poly_closest_point_triangle(float3 point, poly_triangle triangle) {
    const float distance = float3_dot(float3_sub(point, triangle.points[0]), triangle.normal);
    if (float3_dot(triangle.normal, triangle.normal) > 0 && poly_triangle_contains(triangle, point)) {
        return float3_sub(point, float3_scale(triangle.normal, distance));
    }
    poly_closest_pair best = { .distance_squared = INFINITY };
    for (int i = 0; i < 3; i++) {
        poly_keep_closest(&best, point, poly_closest_on_edge(point, triangle.points[i], triangle.points[(i + 1) % 3]));
    }
    return best.triangle;
}

// Both segment endpoints have already been tested against the entire triangle.
static void poly_closest_segment_edge(poly_closest_pair* best, float3 start, float3 end, float3 a, float3 b) {
    poly_keep_closest(best, poly_closest_on_edge(a, start, end), a);
    poly_keep_closest(best, poly_closest_on_edge(b, start, end), b);
    const float3 axis = float3_sub(end, start);
    const float3 edge = float3_sub(b, a);
    const float3 cross = float3_cross(axis, edge);
    const float denominator = float3_dot(cross, cross);
    if (denominator > 0) {
        const float3 delta = float3_sub(a, start);
        const float t = float3_dot(float3_cross(delta, edge), cross) / denominator;
        const float u = float3_dot(float3_cross(delta, axis), cross) / denominator;
        if (t > 0 && t < 1 && u > 0 && u < 1) {
            poly_keep_closest(best, float3_add(start, float3_scale(axis, t)), float3_add(a, float3_scale(edge, u)));
        }
    }
}

static poly_closest_pair poly_closest_segment_triangle(float3 start, float3 end, poly_triangle triangle) {
    const float start_distance = float3_dot(float3_sub(start, triangle.points[0]), triangle.normal);
    const float end_distance = float3_dot(float3_sub(end, triangle.points[0]), triangle.normal);
    if (start_distance != end_distance) {
        const float t = start_distance / (start_distance - end_distance);
        if (t >= 0 && t <= 1) {
            const float3 point = float3_add(start, float3_scale(float3_sub(end, start), t));
            if (poly_triangle_contains(triangle, point)) {
                return (poly_closest_pair) { point, point, 0 };
            }
        }
    }
    poly_closest_pair best = { .distance_squared = INFINITY };
    poly_keep_closest(&best, start, poly_closest_point_triangle(start, triangle));
    poly_keep_closest(&best, end, poly_closest_point_triangle(end, triangle));
    for (int i = 0; i < 3; i++) {
        poly_closest_segment_edge(&best, start, end, triangle.points[i], triangle.points[(i + 1) % 3]);
    }
    return best;
}
