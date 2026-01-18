#include "detect2.c"    // using old (2) atm
#include "unstuck.c"

void define_systems_terrain_collisions(ecs* world) {
    zox_system(
        CollisionDetectSystem,
        zoxp_physics,
        [in] terrain.TerrainLink,
        [in] transforms3.Bounds3D,
        [in] transforms3.Position3D,
        [in] physics3.LastPosition3D,
        [out] collisions3.Collision,
        [out] collisions3.CollisionDistance
    );

    // TODO: Find a better way to modulate this system
    zox_system(
        CollisionResponseSystem,
        zoxp_physics,
        [in] collisions3.CollisionDistance,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D,
        [out] physics3.LastPosition3D,
        [out] collisions3.Collision,
        [out] collisions3.Grounded
    );

    zox_system(
        UnstuckSystem,
        zoxp_physics + 1,
        [in] terrain.TerrainLink,
        [in] transforms3.Bounds3D,
        [out] physics3.LastUnstuck3,
        [out] transforms3.Position3D
    );
}