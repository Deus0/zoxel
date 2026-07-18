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
        zoxp_update,
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
        zoxp_update,
        [in] inputs.MouseLock,
        [in] textures.TextureLink,
        [in] inputs.DeviceDisabled
    );
    zox_filter(fill_texture_query, [none] FillTexture, [out] textures.GenerateTexture)
    zox_system_ctx(
        FillTextureSystem,
        zoxp_update,
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
        zoxp_update,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] OutlineThickness,
        [in] textures.FrameCorner,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.FrameTexture
    );
    add_system_process_counter(world, zox_id(FrameTextureSystem));
    zox_system(
        IconTextureSystem,
        zoxp_update,
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
        zoxp_update,
        [in] textures.TilemapSize,
        [in] textures.TextureLinks,
        [out] textures.GenerateTexture,
        [out] rendering.TextureSize,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] textures.Tilemap
    );
    zox_system_1(
        TextureRgbUploadSystem,
        zoxp_mainthread,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] textures.TextureRGB
    );
    zox_system_1(
        TextureUploadSystem,
        zoxp_mainthread,
        [in] rendering.TextureDirty,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] !textures.TextureRGB
    );
}
