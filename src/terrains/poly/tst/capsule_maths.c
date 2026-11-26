static void zox_tst_capsule_maths_check(ecs* world) {
    (void) world;
    const float epsilon = 1e-5f;
    const poly_triangle triangle = poly_triangle_new(float3_zero, (float3) { 0, 0, 2 }, (float3) { 2, 0, 0 });
    const float3 points[] = { { 0.5f, 1, 0.5f }, { 1.5f, 1, 1.5f }, { -1, 1, -1 } };
    const float3 expected[] = { { 0.5f, 0, 0.5f }, { 1, 0, 1 }, { 0, 0, 0 } };
    for (int i = 0; i < 3; i++) {
        zox_tst_check(zox_tst_float3_error(poly_closest_point_triangle(points[i], triangle), expected[i]) < epsilon,
            "point case %i has the wrong closest point", i);
    }
    const poly_closest_pair crossing = poly_closest_segment_triangle(points[0], (float3) { 0.5f, -1, 0.5f }, triangle);
    zox_tst_check(zox_tst_float3_error(crossing.segment, expected[0]) < epsilon &&
        zox_tst_float3_error(crossing.triangle, expected[0]) < epsilon, "wrong intersection");
    const poly_closest_pair apart = poly_closest_segment_triangle((float3) { -1, 1, 1 }, (float3) { 1, 1, -1 }, triangle);
    zox_tst_check(zox_tst_float3_error(apart.segment, (float3) { 0, 1, 0 }) < epsilon &&
        zox_tst_float3_error(apart.triangle, float3_zero) < epsilon, "wrong closest pair for a separated segment");

    const int2 cells = { 1, 1 };
    const float2 size = { 2, 2 };
    const float flat[] = { 2, 2, 2, 2 };
    const float slope[] = { 0, 2, 0, 2 };
    const float3 up = { 0, 1, 0 };
    float3 push, normal;
    zox_tst_check(poly_capsule_push(flat, cells, size, (float3) { 1, 2.2f, 0.5f }, (float3) { 1, 3.2f, 0.5f },
        0.4f, &push, &normal) && zox_tst_float3_error(normal, up) < epsilon &&
        zox_tst_float3_error(push, float3_scale(up, 0.2f)) < epsilon, "wrong flat overlap");
    zox_tst_check(poly_capsule_push(flat, cells, size, (float3) { 1, 1.5f, 0.5f }, (float3) { 1, 2.5f, 0.5f },
        0.4f, &push, &normal) && fabsf(push.y - 0.9f) < epsilon, "crossing capsule should clear the surface");
    const float3 slope_normal = float3_normalize((float3) { -1, 1, 0 });
    const float3 bottom = float3_add((float3) { 1, 1, 0.5f }, float3_scale(slope_normal, 0.25f));
    zox_tst_check(poly_capsule_push(slope, cells, size, bottom, float3_add(bottom, up), 0.4f, &push, &normal) &&
        zox_tst_float3_error(normal, slope_normal) < epsilon &&
        zox_tst_float3_error(push, float3_scale(slope_normal, 0.15f)) < epsilon, "wrong 45-degree overlap");
    zox_tst_check(zox_log_errors_count == zox_tst_errors_start, "errors logged during capsule_maths");
    zox_tst_check(!poly_capsule_push(flat, cells, size, (float3) { 1, 3, 1 }, (float3) { 1, 4, 1 },
        0.4f, &push, &normal), "an airborne capsule should have no overlap");
}
