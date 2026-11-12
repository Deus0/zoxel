#include "gravity2D_system.c"
#include "acceleration2D_system.c"
#include "velocity2D_system.c"
#include "alpha2D_system.c"
#include "torque2D_system.c"
#include "bounds2D_system.c"

// order per frame: forces (gravity, a controller's) -> an exact step of velocity and position
//  - same frame: a force added before this point moves the body this frame
//  - bodies with an Acceleration2D move in Acceleration2DSystem / Friction2DSystem, the rest
//      in Velocity2DSystem, so nothing moves twice
void zox_systems_physics2(ecs* world) {
    zox_system(
        Gravity2DSystem,
        zoxp_physics,
        [in] Gravity2D,
        [out] Acceleration2D,
        [none] !physics.DisableGravity
    );
    zox_system(
        Acceleration2DSystem,
        zoxp_physics,
        [out] Velocity2D,
        [out] Acceleration2D,
        [out] transforms2.Position2,
        [none] !physics.Frictioned
    );
    zox_system(
        Friction2DSystem,
        zoxp_physics,
        [out] Velocity2D,
        [out] Acceleration2D,
        [out] transforms2.Position2,
        [none] physics.Frictioned
    );
    zox_system(
        Velocity2DSystem,
        zoxp_physics,
        [in] Velocity2D,
        [out] transforms2.Position2,
        [none] !Acceleration2D
    );
    zox_system(
        Alpha2DSystem,
        zoxp_physics,
        [out] Alpha2D,
        [out] Torque2D,
        [out] transforms2.Rotation2
    );
    zox_system(
        Torque2DSystem,
        zoxp_physics,
        [out] transforms2.Rotation2,
        [in] Torque2D,
        [none] !Alpha2D
    );
    zox_system(
        Bounds2DSystem,
        zoxp_physics,
        [in] Bounds2D,
        [out] transforms2.Position2
    );
}
