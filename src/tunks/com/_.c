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
// a tunk's chunks bottom to top, render_distance_y * 2 + 1 of them when it spawned (its own length, the setting can
//  change after): index k is chunk y k - length / 2
zoxc_arrayd(Chunk3Stack, entity);
zoxc_byte(Chunk3Stacked);
// a tunk's top down colours (the first block from the top in each column, shaded by height), a side a power of two
//  (its chunks' depth), made when a map needs them (tunk_map_colors), cleared when its chunks change
zoxc_arrayd(TunkMapColors, color);
// a terrain's maps redraw when this changes (a tunk's colours were cleared)
zoxc_int(TunkMapsVersion);
// Links
zox_tag(TunkLink);

void zox_components_tunks(ecs *world) {
    zoxd_tag(Tunk);
    zoxd_tag(TunkTexture);
    zoxd_tag(RegionTexture);
    zoxd_tag(HeightsTexture);
    zoxd_byte(GenerateTunk);
    zoxd_byte(TunkLod);
    zoxd_int2(TunkPosition);
    zoxd_arrayd(BiomeMap);
    zoxd_arrayd(HeightMap);
    zoxd_arrayd(Chunk3Stack);
    zoxd_byte(Chunk3Stacked);
    zoxd_arrayd(TunkMapColors);
    zoxd_int(TunkMapsVersion);
    zoxd_hashmap(TunkLinks);
    // Links
    zoxd_nf_link(TunkLink);
}
