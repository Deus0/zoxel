zox_tag(Tunk);
zox_tag(TunkTexture);
zox_tag(RegionTexture);
zox_tag(HeightsTexture);
zoxc_byte(GenerateTunk);
zoxc_byte(TunkLod);
zoxc_int2(TunkPosition);
zoxc_arrayd(BiomeMap, byte);
zoxc_arrayd(HeightMap, byte);
zoxc_hashmap(TunkLinks, int2);
// NOTE: this has to be used with render_distance_y
#define chunk3_stack_length render_distance_y_max * 2 + 1
zoxc_array(Chunk3Stack, entity, chunk3_stack_length);
zoxc_byte(Chunk3Stacked);

void define_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_tag(TunkTexture);
    zoxd_tag(RegionTexture);
    zoxd_tag(HeightsTexture);
    zoxd_byte(GenerateTunk);
    zoxd_byte(TunkLod);
    zoxd_int2(TunkPosition);
    zoxd_arrayd(BiomeMap);
    zoxd_arrayd(HeightMap);
    zoxd(Chunk3Stack);
    zoxd_byte(Chunk3Stacked);
    zoxd_hashmap(TunkLinks);
}
