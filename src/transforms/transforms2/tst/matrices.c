// tst: transform2_matrices, each registered 2D matrix path moves a known point and respects DisableTransform.
static void zox_tst_transform2_matrices_check(ecs* world) {
    entity elements[4];
    for (int i = 0; i < 4; i++) {
        elements[i] = zox_new();
        zox_setv(elements[i], Position2, ((float2) { 2, -3 }));
        zox_setv(elements[i], Scale1, 2);
        zox_setv(elements[i], TransformMatrix, float4x4_identity);
    }
    zox_setv(elements[1], Rotation2, M_PI / 2);
    zox_setv(elements[2], Scale2, ((float2) { 3, 4 }));
    zox_add(elements[3], DisableTransform);
    zox_run(world, zox_id(transform2_scale1_system));
    zox_run(world, zox_id(transform2_rotation2_scale1_system));
    zox_run(world, zox_id(transform2_scale2_system));

    const float3 local = { 2, -1, 0 };
    const float3 expected[] = { { 6, -5, 0 }, { 4, 1, 0 }, { 8, -7, 0 }, { 2, -1, 0 } };
    const char* variants[] = { "uniform", "rotated uniform", "XY scale", "disabled" };
    for (int i = 0; i < 4; i++) {
        const float3 world_point = float4x4_multiply_float3(zox_getv(elements[i], TransformMatrix), local);
        zox_tst_check(float3_length(float3_subtract(world_point, expected[i])) < 1e-4f,
            "the [%s] 2D matrix mapped its probe to [%.4f, %.4f, %.4f]", variants[i], world_point.x, world_point.y, world_point.z);
    }
}
