// --------------------------------------------------
// XR Helpers
// --------------------------------------------------

static inline XrQuaternionf float4_to_xr_quaternion(float4 v) {
    return (XrQuaternionf) { v.x, v.y, v.z, v.w };
}

static inline float4 xr_quaternion_to_float4(XrQuaternionf v) {
    return (float4) { v.x, v.y, v.z, v.w };
}

static inline float3 xr_vector_to_float3(XrVector3f v) {
    return (float3) { v.x, v.y, v.z };
}

static inline XrVector3f float3_to_xr_vector(float3 v) {
    return (XrVector3f) { v.x, v.y, v.z };
}

static inline XrFovf float4_to_xr_fov(float4 v) {
    return (XrFovf) { v.x, v.y, v.z, v.w };
}

static inline XrExtent2Di int2_to_xr2(int2 v) {
    return (XrExtent2Di) { v.x, v.y };
}

static inline float4 xr_fov_to_float4(XrFovf v) {
    return (float4) {
        v.angleLeft,
        v.angleRight,
        v.angleDown,
        v.angleUp
    };
}

static inline float4x4 xr_view_to_matrix(XrView view) {
    /*float4 rotation = xr_quaternion_to_float4(
        view.pose.orientation
    );
    rotation.x = -rotation.x;
    rotation.y = -rotation.y;
    return inverse_pr_to_matrix(
        xr_vector_to_float3(view.pose.position),
        rotation
    );*/
    /*return inverse_pr_to_matrix(
        xr_vector_to_float3(view.pose.position),
        quaternion_conjugate(
            xr_quaternion_to_float4(view.pose.orientation)
        )
    );*/
    /*return inverse_pr_to_matrix(
        xr_vector_to_float3(view.pose.position),
        xr_quaternion_to_float4(view.pose.orientation)
    );*/
    return float4x4_transform(
        xr_vector_to_float3(view.pose.position),
        xr_quaternion_to_float4(view.pose.orientation)
    );
}

static inline XrView zox_eye_to_xr_view(
    float3 position,
    float4 rotation,
    float4 fov)
{
    XrView view = {
        .type = XR_TYPE_VIEW,
        .pose = {
            .orientation = float4_to_xr_quaternion(rotation),
            .position = float3_to_xr_vector(position)
        },
        .fov = float4_to_xr_fov(fov)
    };
    return view;
}

static float3 xr_get_head_position(XrView views[2]) {
    return (float3){
        (views[0].pose.position.x +
         views[1].pose.position.x) * 0.5f,
        (views[0].pose.position.y +
         views[1].pose.position.y) * 0.5f,
        (views[0].pose.position.z +
         views[1].pose.position.z) * 0.5f
    };
}

static inline void xr_view_to_eye_data(
    XrView view,
    float4x4* matrix,
    float4* fov)
{
    *matrix = xr_view_to_matrix(view);
    *fov = xr_fov_to_float4(view.fov);
}

static inline int2 xr_config_to_image_size(XrViewConfigurationView config) {
    return (int2) {
        config.recommendedImageRectWidth,
        config.recommendedImageRectHeight
    };
}

static inline XrPosef float4x4_to_xr_pose(float4x4 matrix) {
    // the matrix's rotation, its inverse's position (as inverse_matrix_to_pr gave, checked equal): a mixed pose, check
    //  on a headset whether OpenXR wants the eye's pose (its rotation and position) or the view's (both inverted)
    const float4 rotation = matrix_to_rotation(matrix);
    const float3 position = float4x4_get_position(float4x4_inverse(matrix));
    return (XrPosef) {
        .orientation = float4_to_xr_quaternion(rotation),
        .position = float3_to_xr_vector(position)
    };
}

// --------------------------------------------------
// XR Math
// --------------------------------------------------
