#include "animate_noise_system.c"
#include "texture_rgb_upload_system.c"
#include "texture_rgba_upload_system.c"
#include "noise.c"
#include "fill.c"
#include "frame.c"
#include "tilemap.c"
#include "mouse.c"
#include "icon.c"

void define_systems_textures(ecs *world) {
    zox_system(
        MouseTextureSystem,
        EcsOnUpdate,
        [in] inputs.MouseLock,
        [in] textures.TextureLink,
        [in] inputs.DeviceDisabled
    );
    zox_filter(
        generate_textures2,
        [none] FrameTexture,
        [out] textures.GenerateTexture
    );
    zox_system(
        AnimateNoiseSystem,
        zox_pip_texture_generation,
        [out] AnimateTexture,
        [out] textures.GenerateTexture
    );
    zox_filter(fill_texture_query, [none] FillTexture, [out] textures.GenerateTexture)
    zox_system_ctx(
        FillTextureSystem,
        zoxp_textures,
        fill_texture_query,
        [in] rendering.TextureSize,
        [in] colorz.Color,
        [out] textures.TextureData,
        [out] textures.GenerateTexture,
        [out] rendering.TextureDirty,
        [none] FillTexture
    );
    zox_system_ctx(
        FrameTextureSystem,
        zox_pip_texture_generation,
        generate_textures2,
        [in] rendering.TextureSize,
        [in] colorz.Color,
        [in] OutlineThickness,
        [in] FrameCorner,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] FrameTexture
    );
    zox_system(
        IconTextureSystem,
        zox_pip_texture_generation,
        [in] rendering.TextureSize,
        [in] colorz.Color,
        [in] OutlineColor,
        [in] OutlineThickness,
        [in] IconRadius,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] IconTexture
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
        [none] Tilemap
    );
    zox_system_1(
        TextureUpdateSystem,
        EcsPreStore,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] !TextureRGB
    );
    zox_system_1(
        TextureRGBUpdateSystem,
        EcsPreStore,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] TextureRGB
    );
}
