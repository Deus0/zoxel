#include "unstuck.c"
#include "inside_sound.c"
#include "inside.c"
// #include "detect_new.c"
// #include "detect1.c"
#include "intersect.c"
#include "debug.c"

void define_systems_terrain_collisions(ecs* world) {
    zox_system(
        InsideBlockSystem,
        zoxp_update,
        [in] terrains.TerrainLink,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [out] blocks.InsideBlock,
        [out] blocks.InsideBlockPosition,
        [out] blocks.InsideBlockDirty,
    );
    zox_system_1(
        InsideBlockSoundSystem,
        zoxp_mainthread,
        [in] terrains.TerrainLink,
        [in] blocks.InsideBlockDirty,
        [in] blocks.InsideBlock,
        [in] transforms3.Position3D,
        [out] blocks.InsideBlockTime,
        [none] blocks.BlockSoundTrigger
    );
    zox_system(
        TerrainIntersectSystem,
        zoxp_physics,
        [in] terrains.TerrainLink,
        [in] transforms3.Rotation3D,
        [in] transforms3.Bounds3D,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D,
        [out] physics.Grounded,
        [none] !physics.NoClip
    );
    zox_system_1(
        TerrainIntersectDebugSystem,
        zoxp_mainthread,
        [in] terrains.TerrainLink,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] transforms3.Bounds3D,
    );
    /*byte use_old_collisions = 0;
    if (use_old_collisions) {
        zox_system(
            CollisionDetectSystem,
            zoxp_physics,
            [in] terrains.TerrainLink,
            [in] transforms3.Bounds3D,
            [in] transforms3.Position3D,
            [in] physics3.LastPosition3D,
            [out] collisions3.Collision,
            [out] collisions3.CollisionDistance,
            [none] !physics.NoClip
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
            [out] physics.Grounded
        );
        #ifdef zox_dbg_lines_unstuck
        zox_system_1(
        #else
        zox_system(
        #endif
            UnstuckSystem,
            zoxp_physics + 1,
            [in] terrains.TerrainLink,
            [in] transforms3.Bounds3D,
            [out] physics3.LastUnstuck3,
            [out] transforms3.Position3D,
            [none] !physics.NoClip
        );
    } else {*/
    //}
}
