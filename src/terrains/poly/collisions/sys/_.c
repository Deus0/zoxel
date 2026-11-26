#include "capsule.c"

static void zox_systems_poly_collisions(ecs* world) {
    zox_system(
        poly_capsule_collision_system,
        zoxp_physics,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D,
        [out] physics.Grounded,
        [none] !physics.NoClip
    );
}
