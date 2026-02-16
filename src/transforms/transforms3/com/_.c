zox_tag(EulerOverride);
zox_tag(CameraTransform);
zoxc_float3(Position3D);
zoxc_float4(Rotation3D);     // A quaternion rotation
zoxc_float3(Scale3D);          // A Non Uniform 3D Scale
zoxc(Matrix, float4x4);       // A matrix used for rendering
zoxc_float3(Euler);          // A rotation but kept in euler form
zoxc_float2(EulerLimitX);    // a limitation of euler x axis
zoxc_float2(EulerLimitZ);    // a limitation of euler z axis
zoxc_float3(LocalPosition3D);
zoxc_float4(LocalRotation3D);
zoxc(TransformMatrix, float4x4);
// used for physics
zoxc_float3(Bounds3D);
zoxc_state(Bounds3Dirty);
zoxc_float6(Position3DBounds);
zoxc_entity(ShadowLink);

void define_components_transforms3(ecs* world) {
    zoxd_tag(EulerOverride);
    zoxd_tag(CameraTransform);
    zoxd_float3(Position3D);
    zoxd_float4(Rotation3D);
    zoxd_float3(Scale3D);
    zoxd(Matrix);
    zoxd_float3(Euler);
    zoxd_float2(EulerLimitX);
    zoxd_float2(EulerLimitZ);
    zoxd_float3(LocalPosition3D);
    zoxd_float4(LocalRotation3D);
    zoxd(TransformMatrix);
    // physics
    zoxd_float3(Bounds3D);
    zoxd_state(Bounds3Dirty);
    zoxd_float6(Position3DBounds);
    zoxd_entity(ShadowLink);
}
