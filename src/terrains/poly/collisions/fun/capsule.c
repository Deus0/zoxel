static float3 poly_collision_vertex(const float* heights, int2 cells, float2 size, int x, int z) {
    return (float3) {
        size.x * ((float) x / cells.x),
        poly_height_index(heights, cells, x, z),
        size.y * ((float) z / cells.y)
    };
}

static float poly_capsule_depth(float3 bottom, float3 top, float radius, poly_triangle triangle) {
    const poly_closest_pair pair = poly_closest_segment_triangle(bottom, top, triangle);
    const float distance = sqrtf(pair.distance_squared);
    const float bottom_side = float3_dot(float3_sub(bottom, triangle.points[0]), triangle.normal);
    const float top_side = float3_dot(float3_sub(top, triangle.points[0]), triangle.normal);
    // A vertical axis through the face needs its bottom cap above the plane.
    if (pair.distance_squared == 0 && fminf(bottom_side, top_side) < 0) {
        return radius - fminf(bottom_side, top_side);
    }
    return radius - distance;
}

static byte poly_capsule_push(const float* heights, int2 cells, float2 size, float3 bottom, float3 top,
    float radius, float3* push, float3* normal) {
    const float left = fminf(bottom.x, top.x) - radius;
    const float right = fmaxf(bottom.x, top.x) + radius;
    const float front = fminf(bottom.z, top.z) - radius;
    const float back = fmaxf(bottom.z, top.z) + radius;
    if (!heights || cells.x <= 0 || cells.y <= 0 || size.x <= 0 || size.y <= 0 || radius <= 0 ||
        right < 0 || back < 0 || left > size.x || front > size.y) {
        return 0;
    }
    const int x0 = int_min((int) (clampf(left / size.x, 0, 1) * cells.x), cells.x - 1);
    const int x1 = int_min((int) (clampf(right / size.x, 0, 1) * cells.x), cells.x - 1);
    const int z0 = int_min((int) (clampf(front / size.y, 0, 1) * cells.y), cells.y - 1);
    const int z1 = int_min((int) (clampf(back / size.y, 0, 1) * cells.y), cells.y - 1);
    float deepest = 0;
    for (int z = z0; z <= z1; z++) {
        for (int x = x0; x <= x1; x++) {
            const float3 a = poly_collision_vertex(heights, cells, size, x, z);
            const float3 b = poly_collision_vertex(heights, cells, size, x + 1, z);
            const float3 c = poly_collision_vertex(heights, cells, size, x + 1, z + 1);
            const float3 d = poly_collision_vertex(heights, cells, size, x, z + 1);
            const poly_triangle triangles[] = { poly_triangle_new(a, c, b), poly_triangle_new(a, d, c) };
            for (int i = 0; i < 2; i++) {
                const float depth = poly_capsule_depth(bottom, top, radius, triangles[i]);
                if (depth > deepest) {
                    deepest = depth;
                    *normal = triangles[i].normal;
                }
            }
        }
    }
    if (deepest > 0) {
        *push = float3_scale(*normal, deepest);
        return 1;
    }
    return 0;
}
