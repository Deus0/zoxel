zox_tag(Region);
zoxc_byte(GenerateRegion);
zoxc_int2(RegionPosition);
zoxc_int2(RegionSize);
zoxc_entity(RegionLink);
zoxc_hashmap(RegionLinks, int2)

void zox_define_components_regions(ecs* world) {
    zoxd_tag(Region);
    zoxd_byte(GenerateRegion);
    zoxd_int2(RegionPosition);
    zoxd_int2(RegionSize);
    zoxd_entity(RegionLink);
    zoxd_hashmap(RegionLinks);
}
