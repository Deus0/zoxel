#include "animate_noise_system.c"
#include "upload.c"
#include "noise.c"
#include "fill.c"
#include "frame.c"
#include "tilemap.c"
#include "mouse.c"
#include "icon.c"
#include "arrow.c"
#include "plus.c"
#include "gpu.c"

void zox_systems_textures(ecs *world) {
    zox_system(
        arrow_texture_system,
        zoxp_update,
        [in] core.Generate,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [out] textures.TextureData,
        [none] textures.ArrowTexture
    );
    zox_system(
        plus_texture_system,
        zoxp_update,
        [in] core.Generate,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [out] textures.TextureData,
        [none] textures.PlusTexture
    );
    zox_system(
        mouse_texture_system,
        zoxp_update,
        [in] inputs.MouseLock,
        [none] inputs.Mouse,
    );
    zox_filter(fill_texture_query, [none] FillTexture, [out] textures.GenerateTexture)
    zox_system_ctx(
        fill_texture_system,
        zoxp_update,
        fill_texture_query,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [out] textures.TextureData,
        [out] textures.GenerateTexture,
        [none] textures.FillTexture
    );
    zox_system(
        frame_texture_system,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [in] textures.FrameCorner,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] textures.FrameTexture
    );
    add_system_process_counter(world, zox_id(frame_texture_system));
    zox_system(
        icon_texture_system,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] textures.OutlineThickness,
        [in] textures.IconRadius,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] textures.IconTexture
    );
    zox_system(
        tilemap_generation_system,
        zoxp_update,
        [in] textures.TilemapSize,
        [out] textures.GenerateTexture,
        [out] rendering.TextureSize,
        [out] textures.TextureData,
        [none] textures.Tilemap
    );
    zox_system_1(
        texture_rgb_upload_system,
        zoxp_gpu_upload,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] textures.TextureDirty,
        [none] textures.TextureRGB,
        [none] !core.Initialize,
    );
    zox_system_1(
        texture_rgba_upload_system,
        zoxp_gpu_upload,
        [in] textures.TextureData,
        [in] rendering.TextureSize,
        [in] rendering.TextureGPULink,
        [none] textures.TextureDirty,
        [none] !textures.TextureRGB,
        [none] !core.Initialize,
    );
    zox_gpu_restore_system(
        texture_data_restore_system,
        [none] textures.TextureData,
        [none] textures.Texture
    );
}
