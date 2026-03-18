#include "detect2.c"    // using old (2) atm
#include "unstuck.c"
#include "inside_sound.c"

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

#ifdef zox_dbg_lines_unstuck
    zox_system_1(
#else
    zox_system(
#endif
        UnstuckSystem,
        zoxp_physics + 1,
        [in] terrain.TerrainLink,
        [in] transforms3.Bounds3D,
        [out] physics3.LastUnstuck3,
        [out] blocks.InsideBlock,
        [out] blocks.InsideBlockDirty,
        [out] transforms3.Position3D
    );

    zox_system_1(
        InsideBlockSoundSystem,
        EcsOnUpdate,
        [in] terrain.TerrainLink,
        [in] blocks.InsideBlockDirty,
        [in] blocks.InsideBlock,
        [in] transforms3.Position3D,
        [none] blocks.BlockSoundTrigger
    );
}
