entity spawn_prefab_camera(ecs *world) {
    float3 euler = (float3) { 0, 180, 0 };
    zox_prefab();
    zox_prefab_name("camera");
    zox_add(e, Camera);
    zox_add(e, PreInitialize);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, Euler, euler);
    zox_setv(e, LocalRotation3D, quaternion_from_euler(float3_scale(euler, degreesToRadians)));
    // Camera
    zox_setv(e, CameraState, 0);
    zox_setv(e, ProjectionMatrix, float4x4_identity);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, ViewProjectionMatrix, float4x4_identity);
    zox_setv(e, ScreenPosition, int2_zero);
    zox_setv(e, PixelSize, int2_zero);
    zox_setv(e, FieldOfView, 0);
    zox_setv(e, CameraNearDistance, camera_near_distance);
    zox_setv(e, ScreenToCanvas, float4_zero);
    zox_setv(e, FogColor, color_rgb_green);
    // culling
    zox_setv(e, Position3DBounds, float6_zero);
    zox_add(e, CameraPlanes);
    zox_add(e, FrustumCorners);
    return e;
}
