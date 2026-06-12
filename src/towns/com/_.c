zox_tag(Town);
zoxc_arrayd(TownMap, byte)
zoxc_byte(WallHeight);
zoxc_byte2(TownSize);

void zox_define_components_towns(ecs* world) {
    zoxd_tag(Town);
    zoxd_arrayd(TownMap);
    zoxd_byte(WallHeight);
    zoxd_byte2(TownSize);
}
