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
zox_tag(PlusTexture);
zox_tag(WaterTexture);      // animated ripples (gen/water.c)
zoxc_byte(GenerateTexture);
zoxc_byte(OutlineThickness);
zoxc_byte(FrameCorner);
zoxc_float(IconRadius);
zoxc_double(AnimateTexture);
// zoxc_uint(TilemapIndex);
zoxc_int2(TilemapSize);
zoxc_color(FillColor);
zoxc_color(OutlineColor);
zox_tag(TextureLink);
zox_tag(TilemapTextureLinks); // links a tilemap to all its block textures, in order
zoxc_arrayd(TextureData, color)
zoxc_arrayd(TilemapUVs, float2)
// animated textures: every frame's pixels back to back, TextureData the one shown (texture_animate_system)
zoxc_arrayd(TextureFrames, color)
zoxc_float(TextureFrameRate);   // frames a second
zoxc_byte(TextureFrame);        // the frame it shows (texture_animate_system)

// the frame an animation shows at a time
static inline byte texture_frame_at(double time, float rate, int count) {
    return count > 0 ? (byte) ((long) (time * rate) % count) : 0;
}
// zoxc_entities(TextureLinks)
// Voxes
zox_tag(VoxTexture);
zox_tag(CenterVoxTexture);

void zox_components_textures(ecs* world) {
    // entity types
    zoxd_tag(Texture);    // RGBA
    zoxd_tag(TextureRGB);
    zoxd_tag(Tilemap);
    zoxd_tag(VoxTexture);
    // Properties
    zoxd_tag(FixToLayout);
    // Events
    zoxd_tag(TextureDirty);
    zoxd_byte(GenerateTexture);
    // Data
    // zoxd_uint(TilemapIndex);
    zoxd_byte(OutlineThickness);
    zoxd_byte(FrameCorner);
    zoxd_float(IconRadius);
    zoxd_double(AnimateTexture);
    zoxd_int2(TilemapSize);
    zoxd_color(FillColor);
    zoxd_color(OutlineColor);
    zoxd_nf_link(TextureLink);
    zoxd_nf_links(TilemapTextureLinks);
    zoxd_arrayd(TextureData);
    zoxd_arrayd(TilemapUVs);
    zoxd_arrayd(TextureFrames);
    zoxd_float(TextureFrameRate);
    zoxd_byte(TextureFrame);
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
    zoxd_tag(PlusTexture);
    zoxd_tag(WaterTexture);
}
