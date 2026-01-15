zox_tag(Tunk);
// zoxc_array(HeightMap, byte, 32 * 32);
zoxc_arrayd(HeightMap, byte)

void define_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_arrayd(HeightMap);
}