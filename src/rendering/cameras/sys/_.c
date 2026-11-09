#include "camera_renderer.c"
#include "render3.c"
#include "camera_render_ui_system.c"
#include "init.c"
#include "gpu.c"

void zox_systems_rendering_cameras(ecs *world) {
    // restore
    zox_system_1(
        render_camera_initialize_system,
        zoxp_gpu_upload,
        [in] core.PixelSize,
        [out] render_cameras.FrameBufferLink,
        [out] render_cameras.RenderBufferLink,
        [none] cameras.RenderCamera,
        [none] core.Initialize,
    );
    // Render Camera
    zox_gpu_restore_system(
        render_buffer_restore_system,
        [in] core.PixelSize,
        [out] FrameBufferLink,
        [out] RenderBufferLink
    );
    zox_gpu_restore_system(
        render_texture_restore_system,
        [none] cameras.RenderTexture,
        [none] !rendering.RenderTextureDirty,
    );
    zox_gpu_dispose_system(
        frame_buffer_dispose_system,
        [out] FrameBufferLink
    );
    zox_gpu_dispose_system(
        render_buffer_dispose_system,
        [out] RenderBufferLink
    );
    // rendering
    zox_system_1(
        camera_render3_system,
        zoxp_render,
        [in] cameras.ViewProjectionMatrix,
        [in] transforms3.Position3D,
        [in] cameras.FieldOfView,
        [in] cameras.ScreenPosition,
        [in] core.PixelSize,
        [in] colorz.FogColor,
        [none] !cameras.CameraUI
    );
    zox_system_1(
        camera_render_ui_system,
        zoxp_render,
        [in] cameras.ViewProjectionMatrix,
        [in] transforms3.Position3D,
        [in] cameras.FieldOfView,
        [in] cameras.ScreenPosition,
        [in] core.PixelSize,
        [in] colorz.FogColor,
        [none] cameras.CameraUI
    );
}
