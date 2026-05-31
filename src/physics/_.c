#ifndef zoxm_physics
#define zoxm_physics

zox_tag(Moveable);
zox_tag(Frictioned);
zoxc_byte(DisableMovement);
zoxc_byte(DisableGravity);
zoxc_byte(CollisionDisabled);
#include "physics2/_.c"
#include "physics3/_.c"

zox_begin_module(Physics) {
    zoxd_tag(Moveable);
    zoxd_tag(Frictioned);
    zoxd_byte(DisableMovement);
    zoxd_byte(DisableGravity);
    zoxd_byte(CollisionDisabled);
    zox_import_module(Physics2);
    zox_import_module(Physics3);
} zox_end_module(Physics);

#endif
