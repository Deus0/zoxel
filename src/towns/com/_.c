zox_tag(Town);
zoxc_arrayd(TownMap, byte);
zoxc_byte(GenerateTown);
zoxc_byte(WallThickness);
zoxc_byte(WallHeight);
zoxc_byte2(TownSize);
zoxc_byte(TownGateSide);    // the wall its gate is in: 0 +z, 1 +x, 2 -z, 3 -x (fun/build.c's facings), from its seed
zoxc_byte(TownGateAt);      // its gate along that wall, blocks from its middle plus 128
zox_tag(TownTexture);
zox_tag(Home);
zoxc_byte2(HomeSize);
zoxc_byte(HomeHeight);
zoxc_byte(HomeStyle);   // its roof: 0 flat, 1 peaked (fun/home.c), from its seed
zoxc_byte(HomeFloor);   // its floor's height, the terrain at its middle (the ground under it is levelled to it)
zoxc_byte(HomeFacing);  // its door faces: 0 +z, 1 +x, 2 -z, 3 -x (fun/build.c), to its town's middle
#include "types.c"

void zox_components_towns(ecs* world) {
    zoxd_tag(Town);
    zoxd_arrayd(TownMap);
    zoxd_byte(GenerateTown);
    zoxd_byte(WallThickness);
    zoxd_byte(WallHeight);
    zoxd_byte2(TownSize);
    zoxd_byte(TownGateSide);
    zoxd_byte(TownGateAt);
    zoxd_tag(TownTexture);
    zoxd_tag(Home);
    zoxd_byte2(HomeSize);
    zoxd_byte(HomeHeight);
    zoxd_byte(HomeStyle);
    zoxd_byte(HomeFloor);
    zoxd_byte(HomeFacing);
}
