zox_tag(Tunk);
zox_tag(TunkTexture);
zox_tag(RegionTexture);
zoxc_entity(TunkLink);
zoxc_int2(TunkPosition);
zoxc_arrayd(BiomeMap, byte)
zoxc_arrayd(HeightMap, byte)
zoxc_arrayd(VegetationMap, byte)
zoxc_hashmap(TunkLinks, int2);

// NOTE: this has to be used with render_distance_y
#define chunk3_stack_length 4 * 2 + 1
zoxc_array(Chunk3Stack, entity, chunk3_stack_length);

void define_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_tag(TunkTexture);
    zoxd_tag(RegionTexture);
    zoxd_entity(TunkLink);
    zoxd_int2(TunkPosition);
    zoxd_arrayd(BiomeMap);
    zoxd_arrayd(HeightMap);
    zoxd_arrayd(VegetationMap);
    zoxd(Chunk3Stack);
    zoxd_hashmap(TunkLinks);
}
