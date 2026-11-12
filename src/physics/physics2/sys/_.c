#include "gravity2D_system.c"
#include "acceleration2D_system.c"
#include "velocity2D_system.c"
#include "alpha2D_system.c"
#include "torque2D_system.c"
#include "bounds2D_system.c"

// order per frame: forces (gravity, a controller's) -> an exact step of velocity and position
//  - same frame: a force added before this point moves the body this frame
//  - bodies with an Acceleration2D move in acceleration2d_system / friction2d_system, the rest
//      in velocity2d_system, so nothing moves twice
void zox_systems_physics2(ecs* world) {
    zox_system(
        gravity2d_system,
        zoxp_physics,
        [in] Gravity2D,
        [out] Acceleration2D,
        [none] !physics.DisableGravity
    );
    zox_system(
        acceleration2d_system,
        zoxp_physics,
        [out] Velocity2D,
        [out] Acceleration2D,
        [out] transforms2.Position2,
        [none] !physics.Frictioned
    );
    zox_system(
        friction2d_system,
        zoxp_physics,
        [out] Velocity2D,
        [out] Acceleration2D,
        [out] transforms2.Position2,
        [none] physics.Frictioned
    );
    zox_system(
        velocity2d_system,
        zoxp_physics,
        [in] Velocity2D,
        [out] transforms2.Position2,
        [none] !Acceleration2D
    );
    zox_system(
        alpha2d_system,
        zoxp_physics,
        [out] Alpha2D,
        [out] Torque2D,
        [out] transforms2.Rotation2
    );
    zox_system(
        torque2d_system,
        zoxp_physics,
        [out] transforms2.Rotation2,
        [in] Torque2D,
        [none] !Alpha2D
    );
    zox_system(
        bounds2d_system,
        zoxp_physics,
        [in] Bounds2D,
        [out] transforms2.Position2
    );
}
