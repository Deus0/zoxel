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
    // the eye in the tracking space: its pose as a transform
    return float4x4_transform(
        xr_vector_to_float3(view.pose.position),
        xr_quaternion_to_float4(view.pose.orientation)
    );
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


