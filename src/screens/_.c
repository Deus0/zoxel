#ifndef zoxm_screens
#define zoxm_screens

// TODO: Screen Entities

zox_tag(Screen);
zoxc_int2(ScreenDimensions);
zoxc_byte(ScreenOrientation);

#include "fun/_.c"

zox_begin_module(Screens)
    zoxd_tag(Screen);
    zoxd_byte(ScreenOrientation);
    zoxd_int2(ScreenDimensions);
zox_end_module(Screens);

#endif
