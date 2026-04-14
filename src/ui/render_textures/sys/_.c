#include "initialize.c"
#include "resize.c"
#include "renderer.c"
#include "screen.c"
#include "camera_blur.c"

void define_systems_render_textures(ecs* world) {

    zox_system(
        ScreenRenderTextureSystem,
        EcsOnLoad,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [none] render.textures.RenderTextureScreen
    );

    if (headless) {
        return;
    }

    zox_system_1(
        RenderTextureBeginSystem,
        EcsPreUpdate,
        [in] elements.InitializeElement,
        [in] rendering.TextureSize,
        [in] cameras.CameraLink,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture
    );

    zox_system(
        RenderTextureSizeSystem,
        EcsOnUpdate,
        [in] layouts2.LayoutSizeDirty,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [in] cameras.CameraLink,
        [none] cameras.RenderTexture
    );

    zox_render2D_system(
        RenderTextureRenderSystem,
        [in] transforms.TransformMatrix,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] cameras.RenderTexture
    );

    zox_system_1(
        CameraBlurSystem,
        EcsOnUpdate,
        cameras.CameraBlur,
        cameras.Camera
    );
}
