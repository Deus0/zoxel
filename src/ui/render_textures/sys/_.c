#include "initialize.c"
#include "resize.c"
#include "renderer.c"
#include "screen.c"
#include "camera_blur.c"

void define_systems_render_textures(ecs* world) {
    zox_system(
        ScreenRenderTextureSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [none] render.textures.RenderTextureScreen
    );
    zox_system(
        RenderTextureSizeSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [in] cameras.CameraLink,
        [none] cameras.RenderTexture
    );
    zox_system_1(
        RenderTextureBeginSystem,
        zoxp_mainthread,
        [in] core.Initialize,
        [in] rendering.TextureSize,
        [in] cameras.CameraLink,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture
    );
    /*zox_render2D_system(
        RenderTextureRenderSystem,
        [in] transforms.TransformMatrix,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture
    );*/
    zox_system_1(
        CameraBlurSystem,
        zoxp_mainthread,
        [in] cameras.CameraBlur,
        [in] cameras.CameraVignette,
        [none] cameras.Camera
    );
}
