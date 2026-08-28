zox_tag(Texture);
zox_tag(Tilemap);
zox_tag(FixToLayout);   // snap texture to layout size
zox_tag(TextureDirty);
// types of textures
zox_tag(NoiseTexture);
zox_tag(IconTexture);
zox_tag(FrameTexture);
zox_tag(DirtTexture);
zox_tag(GrassTexture);
zox_tag(SandTexture);
zox_tag(StoneTexture);
zox_tag(ObsidianTexture);
zox_tag(FillTexture);
zox_tag(TextureAddNoise);
zox_tag(TextureRGB);
zox_tag(ArrowTexture);
zoxc_byte(GenerateTexture);
zoxc_byte(OutlineThickness);
zoxc_byte(FrameCorner);
zoxc_float(IconRadius);
zoxc_double(AnimateTexture);
zoxc_int2(TilemapSize);
zoxc_color(FillColor);
zoxc_color(OutlineColor);
zoxc_entity(TilemapLink);
zoxc_entity(TextureLink);
zoxc_arrayd(TextureData, color)
zoxc_arrayd(TilemapUVs, float2)
zoxc_entities(TextureLinks)
// Voxes
zox_tag(VoxTexture);
zox_tag(CenterVoxTexture);

void define_components_textures(ecs* world) {
    // entity types
    zoxd_tag(Texture);    // RGBA
    zoxd_tag(TextureRGB);
    zoxd_tag(Tilemap);
    zoxd_tag(VoxTexture);
    // Properties
    zoxd_tag(FixToLayout);
    // Events
    zoxd_nf_tag(TextureDirty);
    zoxd_byte(GenerateTexture);
    // Data
    zoxd_byte(OutlineThickness);
    zoxd_byte(FrameCorner);
    zoxd_float(IconRadius);
    zoxd_double(AnimateTexture);
    zoxd_int2(TilemapSize);
    zoxd_color(FillColor);
    zoxd_color(OutlineColor);
    zoxd_entity(TilemapLink);
    zoxd_entity(TextureLink);
    zoxd_arrayd(TextureData);
    zoxd_arrayd(TilemapUVs);
    zoxd_entities(TextureLinks);
    // Generation Types (obsolete)
    zoxd_tag(NoiseTexture);
    zoxd_tag(IconTexture);
    zoxd_tag(FrameTexture);
    zoxd_tag(DirtTexture);
    zoxd_tag(GrassTexture);
    zoxd_tag(SandTexture);
    zoxd_tag(StoneTexture);
    zoxd_tag(ObsidianTexture);
    zoxd_tag(FillTexture);
    zoxd_tag(TextureAddNoise);
    zoxd_tag(CenterVoxTexture);
    zoxd_tag(ArrowTexture);
}
