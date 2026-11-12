#include "unstuck.c"
#include "inside.c"
#include "intersect2.c"
#include "debug.c"

void zox_systems_terrain_collisions(ecs* world) {
    zox_system(
        inside_block_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [out] blocks.InsideBlock,
        [out] blocks.InsideBlockPosition,
    );
    /*zox_system_1(
        inside_block_sound_system,
        zoxp_spawn,
        [in] blocks.InsideBlock,
        [in] transforms3.Position3D,
        [out] blocks.InsideBlockTime,
        [none] blocks.BlockSoundTrigger,
        [none] blocks.InsideBlockUpdate,
    );*/
    zox_system(
        terrain_collision_system,
        zoxp_physics,
        [in] transforms3.Rotation3D,
        [in] transforms3.Bounds3D,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D,
        [out] physics.Grounded,
        [none] !physics.NoClip
    );
    /*zox_system(
        unstuck_system,
        zoxp_physics + 1,
        [in] transforms3.Bounds3D,
        [out] physics3.LastUnstuck3,
        [out] transforms3.Position3D,
        [none] !physics.NoClip
    );*/
#ifdef zox_debug
    zox_system_1(
        terrain_intersect_debug_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] transforms3.Bounds3D,
    );
    zox_set_enabled(terrain_intersect_debug_system, zox_dbg_intersect);
#endif
    /*byte use_old_collisions = 0;
    if (use_old_collisions) {
        zox_system(
            collision_detect_system,
            zoxp_physics,
            [in] transforms3.Bounds3D,
            [in] transforms3.Position3D,
            [in] physics3.LastPosition3D,
            [out] collisions3.Collision,
            [out] collisions3.CollisionDistance,
            [none] !physics.NoClip
        );
        // TODO: Find a better way to modulate this system
        zox_system(
            collision_response_system,
            zoxp_physics,
            [in] collisions3.CollisionDistance,
            [out] transforms3.Position3D,
            [out] physics3.Velocity3D,
            [out] physics3.LastPosition3D,
            [out] collisions3.Collision,
            [out] physics.Grounded
        );
    } else {*/
    //}
}
