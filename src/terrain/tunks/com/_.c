zox_tag(Tunk);
// zoxc_array(HeightMap, byte, 32 * 32);
zoxc_arrayd(BiomeMap, byte)
zoxc_arrayd(HeightMap, byte)
zoxc_arrayd(VegetationMap, byte)
zoxc_arrayd(TownMap, byte)

void define_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_arrayd(BiomeMap);
    zoxd_arrayd(HeightMap);
    zoxd_arrayd(VegetationMap);
    zoxd_arrayd(TownMap);
}