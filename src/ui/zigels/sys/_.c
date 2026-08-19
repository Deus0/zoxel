#include "font_texture.c"

void zox_define_systems_zigels(ecs* world) {
    zox_filter(
        fonts,
        [none] FontTexture,
        [out] textures.GenerateTexture
    );
    zox_system_ctx(
        FontTextureSystem,
        zoxp_update,
        fonts,
        [in] ZigelIndex,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] rendering.TextureSize,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] FontTexture,
    );
}
