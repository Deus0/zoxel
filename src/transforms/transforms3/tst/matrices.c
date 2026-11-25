// tst: transform3_matrices, rigid, uniform and XYZ matrices map a probe after scale, rotation and translation.
static void zox_tst_transform3_matrices_check(ecs* world) {
    entity bodies[4];
    const float4 yaw = quaternion_from_axis_angle(float3_up, M_PI / 2);
    for (int i = 0; i < 4; i++) {
        bodies[i] = zox_new();
        zox_setv(bodies[i], Position3D, ((float3) { 5, 7, 11 }));
        zox_setv(bodies[i], Rotation3D, yaw);
        zox_setv(bodies[i], TransformMatrix, float4x4_identity);
    }
    zox_setv(bodies[1], Scale1, 2);
    zox_setv(bodies[2], Scale1, 99);
    zox_setv(bodies[2], Scale3, ((float3) { 2, 3, 4 }));
    zox_setv(bodies[3], Scale3, float3_one);
    zox_add(bodies[3], DisableTransform);
    zox_run(world, zox_id(transform_matrix_system));
    zox_run(world, zox_id(transform_matrix_scale_system));
    zox_run(world, zox_id(transform_matrix_scale3_system));

    const float3 local = { 2, -1, 3 };
    const float3 expected[] = { { 8, 6, 9 }, { 11, 5, 7 }, { 17, 4, 7 }, { 2, -1, 3 } };
    const char* variants[] = { "rigid", "uniform", "XYZ scale", "disabled" };
    for (int i = 0; i < 4; i++) {
        const float3 world_point = float4x4_multiply_float3(zox_getv(bodies[i], TransformMatrix), local);
        zox_tst_check(zox_tst_transform3_near(world_point, expected[i]),
            "the [%s] 3D matrix mapped its probe to [%.4f, %.4f, %.4f]", variants[i], world_point.x, world_point.y, world_point.z);
    }
}
