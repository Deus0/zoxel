#include "animate_noise_system.c"
#include "texture_rgb_upload_system.c"
#include "texture_rgba_upload_system.c"
#include "noise.c"
#include "fill.c"
#include "frame.c"
#include "tilemap.c"
#include "mouse.c"
#include "icon.c"
#include "arrow.c"

void define_systems_textures(ecs *world) {
    zox_system(
        ArrowTextureSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.ArrowTexture
    );
    zox_system(
        MouseTextureSystem,
        EcsOnUpdate,
        [in] inputs.MouseLock,
        [in] textures.TextureLink,
        [in] inputs.DeviceDisabled
    );
    zox_system(
        AnimateNoiseSystem,
        zox_pip_texture_generation,
        [out] textures.AnimateTexture,
        [out] textures.GenerateTexture
    );
    zox_filter(fill_texture_query, [none] FillTexture, [out] textures.GenerateTexture)
    zox_system_ctx(
        FillTextureSystem,
        zoxp_textures,
        fill_texture_query,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [out] textures.TextureData,
        [out] textures.GenerateTexture,
        [out] rendering.TextureDirty,
        [none] textures.FillTexture
    );
    zox_system(
        FrameTextureSystem,
        zox_pip_texture_generation,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] OutlineThickness,
        [in] textures.FrameCorner,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.FrameTexture
    );
    zox_system(
        IconTextureSystem,
        zox_pip_texture_generation,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [in] textures.IconRadius,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.IconTexture
    );
    zox_system(
        TilemapGenerationSystem,
        zox_pip_texture_generation,
        [in] textures.GenerateTexture,
        [in] textures.TilemapSize,
        [in] textures.TextureLinks,
        [out] rendering.TextureSize,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.Tilemap
    );
    zox_system_1(
        TextureRGBUpdateSystem,
        zoxp_mainthread,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] textures.TextureRGB
    );
    zox_system_1(
        TextureRGBAUpdateSystem,
        zoxp_mainthread,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] !textures.TextureRGB
    );
}
