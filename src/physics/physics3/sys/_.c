#include "acceleration.c"
#include "velocity.c"
#include "friction.c"
#include "omega.c"
#include "alpha.c"
#include "dissipation.c"
#include "gravity.c"
#include "disabler.c"
#include "bounds.c" //  move to transforms

void zox_systems_physics3(ecs *world) {
    // Force
    zox_system(
        acceleration3_system,
        zoxp_physics_apply,
        [out] physics3.Acceleration3D,
        [out] physics3.Velocity3D
    );
    zox_system(
        alpha3_system,
        zoxp_physics_apply,
        [out] Alpha3D,
        [out] Omega3D
    );
    // core
    zox_system(
        velocity3_system,
        zoxp_physics,
        [in] physics3.Velocity3D,
        [out] transforms3.Position3D
    );
    zox_system(
        Omega3System,
        zoxp_physics,
        [in] Omega3D,
        [out] transforms3.Rotation3D
    );
    // Friction
    zox_system(
        friction3_system,
        zoxp_physics,
        [in] physics.Grounded,
        [in] physics3.Velocity3D,
        [out] physics3.Acceleration3D,
        [none] physics.Frictioned
    );
    zox_system(
        Dissipation3System,
        zoxp_physics,
        [out] Omega3D,
        [none] physics.Frictioned
    );
    // others
    zox_system(
        gravity3_system,
        zoxp_physics,
        [in] physics3.Gravity3D,
        [out] physics3.Acceleration3D,
        [none] !physics.DisableGravity
    );
    zox_system(
        Physics3DDisableSystem,
        zoxp_physics,
        [out] InitializePhysics3D,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D
    );
    zox_system(
        Position3DBoundsSystem,
        zoxp_physics,
        [in] transforms3.Position3DBounds,
        [in] transforms3.Bounds3D,
        [out] transforms3.Position3D
    );
}
