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

static inline XrCompositionLayerProjectionView zox_eye_to_xr_projection_view(
    float3 position,
    float4 rotation,
    float4 fov,
    int2 image_size,
    XrSwapchain swapchain
) {
    XrCompositionLayerProjectionView view = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
        .pose = {
            .orientation = float4_to_xr_quaternion(rotation),
            .position = float3_to_xr_vector(position)
        },
        .fov = float4_to_xr_fov(fov),
        .subImage = {
            .swapchain = swapchain,
            .imageRect = {
                .offset = { 0, 0 },
                .extent = int2_to_xr2(image_size)
            },
            .imageArrayIndex = 0
        }
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

static inline void xr_get_eye_matrix(
    XrView view,
    float3* position,
    float4* rotation,
    float4* fov)
{
    *position =
        xr_vector_to_float3(view.pose.position);
    *rotation =
        xr_quaternion_to_float4(view.pose.orientation);
    *fov =
        xr_fov_to_float4(view.fov);
}

static inline int2 xr_config_to_image_size(XrViewConfigurationView config) {
    return (int2) {
        config.recommendedImageRectWidth,
        config.recommendedImageRectHeight
    };
}

// --------------------------------------------------
// XR Math
// --------------------------------------------------


static float4x4 xr_create_projection_matrix(
    float4 fov,
    float n,
    float z
) {
    const float l = tanf(fov.x);
    const float r = tanf(fov.y);
    const float d = tanf(fov.z);
    const float u = tanf(fov.w);
    return (float4x4){
        .x = {
            2.0f / (r - l),
            0,
            0,
            0
        },
        .y = {
            0,
            2.0f / (d - u),
            0,
            0
        },
        .z = {
            (r + l) / (r - l),
            (d + u) / (d - u),
            -(z + n) / (z - n),
            -1.0f
        },
        .w = {
            0,
            0,
            -(2.0f * z * n) / (z - n),
            0
        }
    };
}