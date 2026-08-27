entity spawn_prefab_camera(ecs *world) {
    float3 euler = (float3) { 0, 180, 0 };
    zox_prefab();
    zox_prefab_name("camera");
    zox_add(e, Camera);
    zox_add(e, PreInitialize);
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, LocalPosition3D, { float3_zero });
    zox_prefab_set(e, Euler, { euler });
    zox_prefab_set(e, LocalRotation3D, { quaternion_from_euler(float3_scale(euler, degreesToRadians)) });
    // Camera
    zox_prefab_set(e, CameraState, { 0 });
    zox_prefab_set(e, ProjectionMatrix, { float4x4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, ViewProjectionMatrix, { float4x4_identity });
    zox_prefab_set(e, ScreenPosition, { int2_zero });
    zox_prefab_set(e, ScreenDimensions, { int2_zero });
    zox_prefab_set(e, FieldOfView, { 0 });
    zox_prefab_set(e, CameraNearDistance, { camera_near_distance });
    zox_prefab_set(e, Roaming, { 0 });
    zox_prefab_set(e, ScreenToCanvas, { float4_zero });
    zox_prefab_set(e, FogColor, { color_rgb_green });
    // culling
    zox_prefab_set(e, Position3DBounds, { float6_zero });
    zox_prefab_add(e, CameraPlanes);
    zox_prefab_add(e, FrustumCorners);
    return e;
}
