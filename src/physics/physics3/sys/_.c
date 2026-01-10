#include "velocity.c"
#include "acceleration.c"
#include "omega.c"
#include "alpha.c"
#include "dissipation.c"
#include "gravity.c"
#include "disabler.c"
#include "bounds.c" //  move to transforms

void define_systems_physics3(ecs *world) {
    // core
    zox_system(
        Velocity3System,
        zoxp_physics,
        [in] physics3.Velocity3D,
        [out] transforms3.Position3D
    );
    zox_system(
        Acceleration3System,
        zoxp_physics,
        [out] physics3.Acceleration3D,
        [out] physics3.Velocity3D
    );
    zox_system(
        Omega3System,
        zoxp_physics,
        [in] Omega3D,
        [out] transforms3.Rotation3D
    );
    zox_system(
        Alpha3System,
        zoxp_physics,
        [out] Alpha3D,
        [out] Omega3D
    );
    // others
    zox_system(
        Physics3DDisableSystem,
        zoxp_physics,
        [out] InitializePhysics3D,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D)
    zox_system(
        Gravity3DSystem,
        zoxp_physics,
        [in] physics3.Gravity3D,
        [in] physics.DisableGravity,
        [out] physics3.Acceleration3D
    );
    zox_system(
        Position3DBoundsSystem,
        zoxp_physics,
        [in] transforms3.Position3DBounds,
        [in] transforms3.Bounds3D,
        [out] transforms3.Position3D
    );
    zox_system(
        Dissipation3System,
        zoxp_physics,
        [out] Omega3D,
        [none] physics.Frictioned
    );
}