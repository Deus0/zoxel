#include "font_texture.c"

void zox_systems_zigels(ecs* world) {
    zox_filter(
        fonts,
        [none] FontTexture,
        [out] textures.GenerateTexture
    );
    zox_system_ctx(
        FontTextureSystem,
        zoxp_update,
        fonts,
        [in] GlyphIndex,
        [in] textures.FillColor,
        [in] textures.OutlineColor,
        [in] rendering.TextureSize,
        [in] glyphs.FontThickness,
        [in] textures.OutlineThickness,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] FontTexture,
    );
}
