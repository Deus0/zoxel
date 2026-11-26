#include "initialize.c"
#include "resize.c"
#include "screen.c"
#include "camera_blur.c"

void zox_systems_render_textures(ecs* world) {
    zox_observer(screen_render_texture_camera_event, EcsOnSet,
        [in] cameras.ScreenToCanvas, [none] cameras.Camera);
    zox_on_add(screen_render_texture_canvas_event, [none] layouts.Canvas, [none] core.Dirty);
    zox_system(
        screen_render_texture_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] rendering.TextureSize,
        [none] render_textures.RenderTextureScreen,
        [none] layouts.LayoutSizeUpdate,
    );
    zox_system(
        render_texture_size_system,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture,
        [none] layouts.LayoutSizeUpdate,
        [none] !core.Initialize,
    );
    zox_system_1(
        render_texture_initialize_system,
        zoxp_gpu_upload,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture,
        [none] rendering.RenderTextureDirty,
        [none] !core.Initialize,
        [none] !core.PreInitialize,     // its texture is made the frame after (Initialize)
    );
    zox_system_1(
        camera_material_system,
        zoxp_gpu_upload,
        [in] cameras.CameraBlur,
        [in] cameras.CameraVignette,
        [none] cameras.Camera
    );
}
