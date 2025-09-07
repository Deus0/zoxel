zox_tag(Camera);
zox_tag(Camera2D);
zox_tag(Camera3D);
zox_tag(FirstPersonCamera);
zox_tag(CameraUI);
zox_tag(ElementBillboard);
zoxc_byte(CanRoam);
zoxc_byte(CameraMode);
zoxc_byte(CameraViewing);
zoxc_float(FieldOfView);
zoxc_float(CameraNearDistance);
zoxc_entity(CameraLink);
zoxc_entity(CameraTarget);
zoxc_entity(CameraFollowLink);
zoxc_int2(ScreenPosition);
zoxc_float4(ScreenToCanvas);
zoxc(ProjectionMatrix, float4x4);
zoxc(ViewMatrix, float4x4); // todo: rename to ViewProjectionMatrix
zoxc_array(CameraPlanes, plane, 6);
#ifdef frustumdouble
    zoxc_array(FrustumCorners, double3, 8);
#else
    zoxc_array(FrustumCorners, float3, 8);
#endif
// Render Cameras
zox_tag(RenderCamera);
zox_tag(RenderTexture);
zoxc_entity(RenderTextureLink);
zoxc_entity(RenderCameraLink);
zoxc_entities(CameraLinks)

void define_components_cameras(ecs *world) {
    zoxd_tag(Camera);
    zoxd_tag(Camera2D);
    zoxd_tag(Camera3D);
    zoxd_tag(FirstPersonCamera);
    zoxd_tag(CameraUI);
    zoxd_tag(ElementBillboard);
    zoxd_byte(CanRoam);
    zoxd_byte(CameraMode);
    zoxd_byte(CameraViewing);
    zoxd_float(FieldOfView);
    zoxd_float(CameraNearDistance);
    zoxd_int2(ScreenPosition);
    zoxd_entity(CameraLink);
    zoxd_entity(CameraTarget);
    zoxd_entity(CameraFollowLink);
    zoxd(ProjectionMatrix);
    zoxd(ViewMatrix);
    // zoxd(ViewProjectionMatrix)
    zoxd_float4(ScreenToCanvas);
    zox_define_component_array(FrustumCorners);
    zox_define_component_array(CameraPlanes);
    // render cameras
    zoxd_tag(RenderCamera);
    zoxd_tag(RenderTexture);
    zoxd_entity(RenderTextureLink);
    zoxd_entity(RenderCameraLink);
    zox_define_entities_component(CameraLinks);
}