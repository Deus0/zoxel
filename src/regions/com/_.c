zox_tag(Region);
zoxc_int2(RegionPosition);
zoxc_entity(RegionLink);
zoxc_hashmap(RegionLinks, int2)

void zox_define_components_regions(ecs* world) {
    zoxd_tag(Region);
    zoxd_int3(RegionPosition);
    zoxd_entity(RegionLink);
    zoxd_hashmap(RegionLinks);
}
