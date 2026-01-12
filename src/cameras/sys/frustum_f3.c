void calculate_frustum_corners_f3(const float4x4 view_projection_matrix, float3 *frustum) {
    frustum[0] = (float3) { -1, -1, -1 };
    frustum[1] = (float3) { 1, -1, -1 };
    frustum[2] = (float3) { 1, 1, -1 };
    frustum[3] = (float3) { -1, 1, -1 };
    frustum[4] = (float3) { -1, -1, 1 };
    frustum[5] = (float3) { 1, -1, 1 };
    frustum[6] = (float3) { 1, 1, 1 };
    frustum[7] = (float3) { -1, 1, 1 };
    const float4x4 inv = float4x4_inverse_safe(view_projection_matrix);
    for (int i = 0; i < 8; i++) {
        const float4 corner = float4_from_float3(frustum[i], 1);
        const float4 world_corner = float4x4_multiply_float4(inv, corner);
        frustum[i] = float3_divide_float(
            (float3) { world_corner.x, world_corner.y, world_corner.z },
            world_corner.w
        );
    }
}

void calculate_frustum_bounds_f3(const float3 *corners, float6 *bounds) {
    bounds->x = bounds->z = bounds->u = FLT_MAX;
    bounds->y = bounds->w = bounds->v = -FLT_MAX;
    for (int i = 0; i < 8; i++) {
        if (corners[i].x < bounds->x) bounds->x = corners[i].x;
        if (corners[i].x > bounds->y) bounds->y = corners[i].x;
        if (corners[i].y < bounds->z) bounds->z = corners[i].y;
        if (corners[i].y > bounds->w) bounds->w = corners[i].y;
        if (corners[i].z < bounds->u) bounds->u = corners[i].z;
        if (corners[i].z > bounds->v) bounds->v = corners[i].z;
    }
}

void frustum_to_planes_f3(float3* frustum, plane* planes) {
    // Left (swap 3 and 4)
    planes[0] = calculate_plane_from_points_f3(frustum[0], frustum[4], frustum[3]);

    // Right (swap 2 and 1)
    planes[1] = calculate_plane_from_points_f3(frustum[5], frustum[1], frustum[2]);

    // Bottom (swap 1 and 0)
    planes[2] = calculate_plane_from_points_f3(frustum[4], frustum[0], frustum[1]);

    // Top (swap 6 and 7)
    planes[3] = calculate_plane_from_points_f3(frustum[3], frustum[7], frustum[6]);

    // Near (swap 2 and 3)
    planes[4] = calculate_plane_from_points_f3(frustum[1], frustum[3], frustum[2]);

    // Far (swap 6 and 5)
    planes[5] = calculate_plane_from_points_f3(frustum[7], frustum[5], frustum[6]);
}

#ifndef frustumdouble

zox_sys2(CameraFrustumSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(ViewMatrix);
    zox_sys_out(FrustumCorners);
    zox_sys_out(Position3DBounds);
    zox_sys_out(CameraPlanes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ViewMatrix, vp_matrix);
        zox_sys_o(CameraPlanes, planes);
        zox_sys_o(FrustumCorners, corners);
        zox_sys_o(Position3DBounds, bounds);

        calculate_frustum_corners_f3(vp_matrix->value, corners->value);
        calculate_frustum_bounds_f3(corners->value, &bounds->value);
        frustum_to_planes_f3(corners->value, planes->value);
    }
} zox_sys_end(CameraFrustumSystem);

#endif