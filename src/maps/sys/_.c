#include "player.c"
#include "spawn.c"
#include "update.c"
#include "arrow.c"
#include "move.c"

void zox_define_systems_maps(ecs* world) {
    zox_system_1(
        PlayerMinimapSystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system_1(
        MapInitializeSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [in] maps.MapZoom,
        [in] rendering.Alpha,
        [in] players.PlayerLink,
        [in] terrains.TerrainLink,
        [in] maps.MapPosition,
        [none] maps.Map
    );
    zox_system_1(
        MapPositionSystem,
        zoxp_mainthread,
        [in] players.PlayerLink,
        [in] terrains.TerrainLink,
        [out] maps.MapPosition,
        [none] maps.Map
        // [none] maps.Minimap
    );
    zox_system(
        MapArrowSystem,
        zoxp_update,
        [in] players.PlayerLink,
        [out] transforms2.Rotation2,
        [none] maps.MapArrow
    );
}
