zox_tag(Screen);
//zoxc_int2(ScreenDimensions);
zoxc_byte(ScreenOrientation);

#include "fun/_.c"

void import_screens(ecs* world) {
    zox_module(screens);
    zoxd_tag(Screen);
    zoxd_byte(ScreenOrientation);
    //zoxd_int2(ScreenDimensions);
}
