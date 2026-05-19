zox_tag(Tunk);
zoxc_entity(TunkLink);
zoxc_arrayd(BiomeMap, byte)
zoxc_arrayd(HeightMap, byte)
zoxc_arrayd(VegetationMap, byte)
zoxc_arrayd(TownMap, byte)

// NOTE: this has to be used with render_distance_y
#define chunk3_stack_length 4 * 2 + 1
zoxc_array(Chunk3Stack, entity, chunk3_stack_length);

void define_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_entity(TunkLink);
    zoxd_arrayd(BiomeMap);
    zoxd_arrayd(HeightMap);
    zoxd_arrayd(VegetationMap);
    zoxd_arrayd(TownMap);
    zoxd(Chunk3Stack);
}