#include "initialize.c"
#include "resize.c"
#include "screen.c"
#include "camera_blur.c"

void define_systems_render_textures(ecs* world) {
    zox_system(
        ScreenRenderTextureSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] layouts.LayoutSize,
        [out] rendering.TextureSize,
        [none] render.textures.RenderTextureScreen
    );
    zox_system(
        RenderTextureSizeSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture,
        [none] !core.Initialize
    );
    zox_system_1(
        RenderTextureInitializeSystem,
        zoxp_gpu_upload,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture,
        [none] rendering.RenderTextureDirty,
        [none] !core.Initialize,
    );
    zox_system_1(
        CameraMaterialSystem,
        zoxp_gpu_upload,
        // [in] rendering.MaterialLink,
        [in] cameras.CameraBlur,
        [in] cameras.CameraVignette,
        [none] cameras.Camera
    );
}
