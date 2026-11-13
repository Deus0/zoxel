zox_tag(Region);
zox_tag(RegionLink);
zoxc_byte(GenerateRegion);
zoxc_int2(RegionPosition);
zoxc_int2(RegionSize);
zoxc_hashmap(RegionLinks, int2)
zox_tag(PlacementZone);     // an area placed in a region (fun/zones.c), a circle (Radius) or a rect (ZoneSize)
zoxc_byte2(ZoneSize);

void zox_components_regions(ecs* world) {
    zoxd_tag(Region);
    zoxd_nf_link(RegionLink);
    zoxd_byte(GenerateRegion);
    zoxd_int2(RegionPosition);
    zoxd_int2(RegionSize);
    zoxd_hashmap(RegionLinks);
    zoxd_tag(PlacementZone);
    zoxd_byte2(ZoneSize);
}
