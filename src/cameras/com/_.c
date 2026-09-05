zox_tag(Camera);
zox_tag(Camera2);
zox_tag(Camera3);
zox_tag(CameraUI);
zox_tag(ElementBillboard);
zox_tag(Perspective);
zox_tag(Orthographic);
zoxc_byte(CameraState);
zoxc_byte(CameraViewing);
zoxc_float(FieldOfView);
zoxc_float(CameraNearDistance);
zoxc_float(CameraFarDistance);
zoxc_entity(CameraTarget);
zoxc_entity(CameraFollowLink);
zoxc_int2(ScreenPosition);
zoxc_float4(ScreenToCanvas);
zoxc(ProjectionMatrix, float4x4);
zoxc(ViewProjectionMatrix, float4x4); // todo: rename to ViewProjectionMatrix
#define zox_camera_planes 6
zoxc_array(CameraPlanes, plane, zox_camera_planes);
zoxc_array(FrustumCorners, double3, 8);
zoxc_state(AttachDirty);
// Filters
zox_tag(CameraFilter);
zoxc_entity(CameraRenderer);
zox_tag(ViewportCamera);
// Render Cameras
zox_tag(RenderCamera);
zox_tag(RenderTexture);
zoxc_entity(RenderTextureLink);
zoxc_entity(RenderCameraLink);
zoxc_float(CameraBlur);
zoxc_float(CameraVignette);
// Properties
zox_tag(CanRoam);
zox_tag(Roaming);

void define_components_cameras(ecs *world) {
    zoxd_tag(Camera);
    zoxd_tag(Camera2);
    zoxd_tag(Camera3);
    zoxd_tag(CameraUI);
    zoxd_tag(ViewportCamera);
    zoxd_tag(ElementBillboard);
    zoxd_tag(Perspective);
    zoxd_tag(Orthographic);
    zoxd_byte(CameraState);
    zoxd_byte(CameraViewing);
    zoxd_float(FieldOfView);
    zoxd_float(CameraNearDistance);
    zoxd_float(CameraFarDistance);
    zoxd_int2(ScreenPosition);
    zoxd_entity(CameraTarget);
    zoxd_entity(CameraFollowLink);
    zoxd(ProjectionMatrix);
    zoxd(ViewProjectionMatrix);
    // zoxd(ViewProjectionMatrix)
    zoxd_float4(ScreenToCanvas);
    zox_define_component_array(FrustumCorners);
    zox_define_component_array(CameraPlanes);
    zoxd_state(AttachDirty);
    // render cameras
    zoxd_tag(RenderCamera);
    zoxd_tag(RenderTexture);
    zoxd_entity(RenderTextureLink);
    zoxd_entity(RenderCameraLink);
    zoxd_float(CameraBlur);
    zoxd_float(CameraVignette);
    // Filters
    zoxd_tag(CameraFilter);
    zoxd_entity(CameraRenderer);
    // temp properties
    zoxd_nf_tag(CanRoam);
    zoxd_nf_tag(Roaming);
}
