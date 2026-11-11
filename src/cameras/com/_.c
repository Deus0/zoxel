zox_tag(Camera);
zox_tag(CameraLink);
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
zox_tag(CameraTarget);     // camera -> the entity it follows
zox_tag(CameraFollowLink);
zoxc_int2(ScreenPosition);
zoxc_float4(ScreenToCanvas);
zoxc(ProjectionMatrix, float4x4);
zoxc(ViewProjectionMatrix, float4x4); // todo: rename to ViewProjectionMatrix
#define zox_camera_planes 6
zoxc_array(CameraPlanes, plane, zox_camera_planes);
zoxc_array(FrustumCorners, double3, 8);
// Filters
zox_tag(CameraFilter);
zox_tag(CameraRenderer);   // entity -> the only camera allowed to render it
zox_tag(ViewportCamera);
// Render Cameras
zox_tag(RenderCamera);
zox_tag(RenderTexture);
zox_tag(RenderTextureLink);    // camera -> the render texture it draws to
zoxc_float(CameraBlur);
zoxc_float(CameraVignette);
// Properties
zox_tag(CanRoam);
zox_tag(Roaming);
zoxc_dirty(Attach);

void zox_components_cameras(ecs *world) {
    zoxd_tag(Camera);
    zoxd_nf_link(CameraLink);
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
    zoxd_nf_link(CameraTarget);
    zoxd_nf_link(CameraFollowLink);
    zoxd(ProjectionMatrix);
    zoxd(ViewProjectionMatrix);
    // zoxd(ViewProjectionMatrix)
    zoxd_float4(ScreenToCanvas);
    zox_define_component_array(FrustumCorners);
    zox_define_component_array(CameraPlanes);
    // render cameras
    zoxd_tag(RenderCamera);
    zoxd_tag(RenderTexture);
    zoxd_nf_link(RenderTextureLink);
    zoxd_float(CameraBlur);
    zoxd_float(CameraVignette);
    // Filters
    zoxd_tag(CameraFilter);
    zoxd_nf_link(CameraRenderer);
    // temp properties
    zoxd_nf_tag(CanRoam);
    zoxd_tag(Roaming);
    zoxd_dirty(Attach);
}
