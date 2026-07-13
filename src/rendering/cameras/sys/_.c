#include "restore_render_buffer_system.c"
#include "render_texture_restore_system.c"
#include "camera_renderer.c"
#include "camera_render3D_system.c"
#include "camera_render_ui_system.c"

void define_systems_rendering_cameras(ecs *world) {
    // restore
    zox_gpu_restore_system(
        RenderBufferRestoreSystem,
        [in] screens.ScreenDimensions,
        [out] FrameBufferLink,
        [out] RenderBufferLink
    );
    zox_gpu_restore_system(
        RenderTextureRestoreSystem,
        [in] rendering.TextureGPULink,
        [in] rendering.TextureSize,
        [in] cameras.CameraLink,
        [none] cameras.RenderTexture
    );
    // rendering
    zox_system_1(
        CameraRender3DSystem,
        zoxp_rendering,
        [in] cameras.ViewMatrix,
        [in] transforms3.Position3D,
        [in] cameras.FieldOfView,
        [in] cameras.ScreenPosition,
        [in] screens.ScreenDimensions,
        [in] colorz.FogColor,
        [none] !cameras.CameraUI
    );
    zox_system_1(
        CameraRenderUISystem,
        zoxp_rendering,
        [in] cameras.ViewMatrix,
        [in] transforms3.Position3D,
        [in] cameras.FieldOfView,
        [in] cameras.ScreenPosition,
        [in] screens.ScreenDimensions,
        [in] colorz.FogColor,
        [none] cameras.CameraUI
    );
}