#ifndef zoxm_transforms2
#define zoxm_transforms2

zoxc_float2(Position2);
zoxc_float(Rotation2D);
zoxc_float2(Scale2D);
#include "fun/_.c"

zox_begin_module(Transforms2) {
    zoxd_float2(Position2);
    zoxd_float(Rotation2D);
    zoxd_float2(Scale2D);
} zox_end_module(Transforms2);

#endif