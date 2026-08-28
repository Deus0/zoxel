// todo: convert camera mouse point to a ray origin and normal
//      > we can use frustum actually, the near and far plane, since we have its corners
//      > camera_point_to_ray

// can we use double3 here? test it
Ray camera_point_to_ray_f3(const float3* frustum_corners, const double2 point) {
    Ray ray;
    // Interpolate point on the near plane
    float3 near_top = float3_lerp(frustum_corners[0], frustum_corners[1], point.x);
    float3 near_bottom = float3_lerp(frustum_corners[3], frustum_corners[2], point.x);
    // Interpolate point on the far plane
    float3 far_top = float3_lerp(frustum_corners[4], frustum_corners[5], point.x);
    float3 far_bottom = float3_lerp(frustum_corners[7], frustum_corners[6], point.x);
    // get points to and from
    float3 near_point = float3_lerp(near_top, near_bottom, point.y);
    float3 far_point = float3_lerp(far_top, far_bottom, point.y);
    // Create ray
    ray.origin = near_point;
    ray.normal = float3_normalize(float3_subtract(far_point, near_point));
    return ray;
}

Ray camera_point_to_ray_d3(const double3* frustum_corners, const double2 point) {
    Ray ray;
    // Interpolate point on the near plane
    const double3 near_top = double3_lerp(frustum_corners[0], frustum_corners[1], point.x);
    const double3 near_bottom = double3_lerp(frustum_corners[3], frustum_corners[2], point.x);
    const double3 far_top = double3_lerp(frustum_corners[4], frustum_corners[5], point.x);
    const double3 far_bottom = double3_lerp(frustum_corners[7], frustum_corners[6], point.x);
    // get points to and from
    const double3 near_point = double3_lerp(near_top, near_bottom, point.y);
    const double3 far_point = double3_lerp(far_top, far_bottom, point.y);
    // Create ray
    ray.origin = double3_to_float3(near_point);
    ray.normal = double3_to_float3(double3_normalize(double3_sub(far_point, near_point)));
    return ray;
}

#ifdef zox_debug_camera_rays
extern void spawn_line3(ecs *world, const float3, const float3, const color_rgb);
#endif

void calculate_new_frustum(
    ecs *world,
    entity e,
    float camera_far,
    double3 *corners)
{
    zox_geter(e, TransformMatrix, matrix);
    const FieldOfView *sysOfView = zox_get(e, FieldOfView);
    const CameraNearDistance *cameraNearDistance = zox_get(e, CameraNearDistance);
    const PixelSize *screenDimensions = zox_get(e, PixelSize);
    float screen_y = (float) screenDimensions->value.y;
    if(screen_y <= 0) screen_y = 1;
    const float aspect_ratio = ((float) screenDimensions->value.x) / ((float) screen_y);
    float4x4 projection_matrix;
    calculate_perspective_projection_matrix(&projection_matrix, aspect_ratio, cameraNearDistance->value, camera_far, sysOfView->value);
    const float4x4 view_matrix = float4x4_inverse(matrix->value);
    const float4x4 view_transform_matrix = float4x4_multiply(view_matrix, projection_matrix);
    calculate_frustum_corners_d3(view_transform_matrix, corners);
}

// remember: frustum is having precision errors for rays
zox_sys2(CameraRaySystem) {
    const float max_ray_distance = 1;
    const double2 viewport_position = (double2) { 0.5, 0.5 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(RaycastOrigin);
    zox_sys_out(RaycastNormal);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(RaycastOrigin, origin);
        zox_sys_o(RaycastNormal, normal);

        // Ray ray = camera_point_to_ray_d3(frustumCorners->value, viewport_position);
        // camera far for ray system, must be small due to limitations of ray math
        // todo: either get normal frustum to work perfectly or bake this so we can cast multiple raycasts with tthe same closer frustum!
        double3 corners[8];
        calculate_new_frustum(world, e, max_ray_distance, corners);
        // this lerps our ray b based on frustum
        Ray ray = camera_point_to_ray_d3(corners, viewport_position);
        origin->value = ray.origin;
        normal->value = ray.normal;
        // zox_log(" > ray [%fx%fx%f] - n [%fx%fx%f]\n", ray.origin.x, ray.origin.y, ray.origin.z, ray.normal.x, ray.normal.y, ray.normal.z)
#ifdef zox_debug_camera_rays
        spawn_line3(world, ray.origin, float3_add(ray.origin, float3_scale(ray.normal, 8)), color_rgb_white);
#endif
    }
} zox_sys_end(CameraRaySystem);
