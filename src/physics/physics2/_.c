zoxc_float2(Velocity2D);     //! Velocity movement of a 2D Entity
zoxc_float2(Acceleration2D); //! Accelaration movement of a 2D Entity, cleared every frame
zoxc_float(Torque2D);        //! Rotational velocity of a 2D Entity (radians per second)
zoxc_float(Alpha2D);         //! Rotational acceleration of a 2D Entity, cleared every frame
zoxc_float2(Gravity2D);      //! Constant acceleration of a 2D Entity (opt in)
zoxc_float2(Bounds2D);       //! Bounding box of a 2D Entity
#include "fun/_.c"
#include "sys/_.c"

void import_physics2(ecs* world) {
    zox_module(physics2);
    zoxd(Velocity2D);
    zoxd(Torque2D);
    zoxd(Alpha2D);
    zoxd(Gravity2D);
    zoxd(Acceleration2D);
    zoxd(Bounds2D);
    zox_systems_physics2(world);
}
