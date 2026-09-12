zox_tag(Region);
zox_tag(RegionLink);
zoxc_byte(GenerateRegion);
zoxc_int2(RegionPosition);
zoxc_int2(RegionSize);
zoxc_hashmap(RegionLinks, int2)

void zox_define_components_regions(ecs* world) {
    zoxd_tag(Region);
    zoxd_nf_tag(RegionLink);
    zoxd_byte(GenerateRegion);
    zoxd_int2(RegionPosition);
    zoxd_int2(RegionSize);
    zoxd_hashmap(RegionLinks);
}
