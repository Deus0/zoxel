zox_tag(Town);
zoxc_arrayd(TownMap, byte);
zoxc_byte(GenerateTown);
zoxc_byte(WallThickness);
zoxc_byte(WallHeight);
zoxc_byte2(TownSize);
zox_tag(TownTexture);
zox_tag(Home);
zoxc_byte2(HomeSize);
zoxc_byte(HomeHeight);
#include "types.c"

void zox_components_towns(ecs* world) {
    zoxd_tag(Town);
    zoxd_arrayd(TownMap);
    zoxd_byte(GenerateTown);
    zoxd_byte(WallThickness);
    zoxd_byte(WallHeight);
    zoxd_byte2(TownSize);
    zoxd_tag(TownTexture);
    zoxd_tag(Home);
    zoxd_byte2(HomeSize);
    zoxd_byte(HomeHeight);
}
