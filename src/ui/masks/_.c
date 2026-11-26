#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_masks(ecs* world) {
    zox_module(masks);
    zoxd_tag(Mask);
    zoxd_int4(LayoutClip);
    zox_observers_masks(world);
}
