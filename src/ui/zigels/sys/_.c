#include "font_texture.c"

void zox_define_systems_zigels(ecs* world) {
    zox_filter(
        fonts,
        [none] FontTexture,
        [out] textures.GenerateTexture
    );
    zox_system_ctx(
        FontTextureSystem,
        zoxp_text,
        fonts,
        [in] textures.GenerateTexture,
        [in] ZigelIndex,
        [in] colorz.Color,
        [in] colorz.SecondaryColor,
        [in] rendering.TextureSize,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] FontTexture
    );
}
