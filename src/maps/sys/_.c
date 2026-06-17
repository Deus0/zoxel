#include "player.c"
#include "spawn.c"
#include "update.c"
#include "arrow.c"

void zox_define_systems_maps(ecs* world) {
    zox_system_1(
        PlayerMinimapSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system_1(
        MapInitializeSystem,
        EcsOnUpdate,
        [in] core.InitializeEntity,
        [in] players.PlayerLink,
        [in] terrains.TerrainLink,
        [in] maps.MapPosition,
        [none] maps.Minimap
    );
    zox_system_1(
        MapPositionSystem,
        EcsOnUpdate,
        [in] players.PlayerLink,
        [in] terrains.TerrainLink,
        [out] maps.MapPosition,
        [none] maps.Minimap
    );
    zox_system(
        MapArrowSystem,
        EcsOnUpdate,
        [in] players.PlayerLink,
        [out] transforms2.Rotation2D,
        [none] maps.MapArrow
    );
}
