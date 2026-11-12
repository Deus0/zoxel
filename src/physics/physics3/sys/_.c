#include "acceleration.c"
#include "velocity.c"
#include "rolling.c"
#include "alpha.c"
#include "gravity.c"
#include "disabler.c"
#include "bounds.c" //  move to transforms

// all in zoxp_physics, one frame:
//  - forces: gravity (and anything added before zoxp_physics: controllers, ai in zoxp_update)
//  - an exact step of velocity and position (see physics/fun), friction included
//  - spin: rolling sets it, then an exact step of omega and rotation, angular drag included
//  - forces added in zoxp_physics by later modules (i.e. jump_force_system) land next frame
//  - each body moves in exactly one linear and one spin system, picked by its components
void zox_systems_physics3(ecs *world) {
    zox_system(
        gravity3_system,
        zoxp_physics,
        [in] physics3.Gravity3D,
        [out] physics3.Acceleration3D,
        [none] !physics.DisableGravity
    );
    zox_system(
        physics3d_disable_system,
        zoxp_physics,
        [out] InitializePhysics3D,
        [out] physics3.Velocity3D,
        [out] physics3.Acceleration3D
    );
    // linear: not Frictioned, or Frictioned without a Grounded (no drag)
    zox_system(
        acceleration3_system,
        zoxp_physics,
        [out] physics3.Acceleration3D,
        [out] physics3.Velocity3D,
        [out] transforms3.Position3D,
        [none] !physics.Frictioned
    );
    zox_system(
        acceleration3_loose_system,
        zoxp_physics,
        [out] physics3.Acceleration3D,
        [out] physics3.Velocity3D,
        [out] transforms3.Position3D,
        [none] physics.Frictioned,
        [none] !physics.Grounded
    );
    zox_system(
        friction3_system,
        zoxp_physics,
        [in] physics.Grounded,
        [out] physics3.Acceleration3D,
        [out] physics3.Velocity3D,
        [out] transforms3.Position3D,
        [none] physics.Frictioned
    );
    zox_system(
        velocity3_system,
        zoxp_physics,
        [in] physics3.Velocity3D,
        [out] transforms3.Position3D,
        [none] !physics3.Acceleration3D
    );
    // spin
    zox_system(
        rolling3_system,
        zoxp_physics,
        [in] physics.Grounded,
        [in] physics3.Velocity3D,
        [in] transforms3.Bounds3D,
        [in] transforms3.Rotation3D,
        [out] physics3.Omega3D,
        [none] physics3.Rolling3D,
        [none] !physics.Frictioned
    );
    zox_system(
        rolling3_damped_system,
        zoxp_physics,
        [in] physics.Grounded,
        [in] physics3.Velocity3D,
        [in] transforms3.Bounds3D,
        [in] transforms3.Rotation3D,
        [out] physics3.Omega3D,
        [none] physics3.Rolling3D,
        [none] physics.Frictioned
    );
    zox_system(
        spin3_system,
        zoxp_physics,
        [out] physics3.Alpha3D,
        [out] physics3.Omega3D,
        [out] transforms3.Rotation3D,
        [none] !physics.Frictioned
    );
    zox_system(
        spin3_damped_system,
        zoxp_physics,
        [out] physics3.Alpha3D,
        [out] physics3.Omega3D,
        [out] transforms3.Rotation3D,
        [none] physics.Frictioned
    );
    zox_system(
        omega3_system,
        zoxp_physics,
        [out] physics3.Omega3D,
        [out] transforms3.Rotation3D,
        [none] !physics3.Alpha3D,
        [none] !physics.Frictioned
    );
    zox_system(
        omega3_damped_system,
        zoxp_physics,
        [out] physics3.Omega3D,
        [out] transforms3.Rotation3D,
        [none] !physics3.Alpha3D,
        [none] physics.Frictioned
    );
    zox_system(
        position3d_bounds_system,
        zoxp_physics,
        [in] transforms3.Position3DBounds,
        [in] transforms3.Bounds3D,
        [out] transforms3.Position3D
    );
}
