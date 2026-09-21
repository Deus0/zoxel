#include "spawn.c"
#include "update.c"
#include "arrow.c"
#include "move.c"

void zox_systems_maps(ecs* world) {
    zox_system_1(
        MapInitializeSystem,
        zoxp_spawn,
        [in] maps.MapZoom,
        [in] rendering.Alpha,
        [in] maps.MapPosition,
        [none] maps.Map,
        [none] core.Initialize,
    );
    zox_system_1(
        MapPositionSystem,
        zoxp_spawn,
        [out] maps.MapPosition,
        [none] maps.Map,
        // [none] maps.Minimap
    );
    zox_system(
        MapArrowSystem,
        zoxp_update,
        [out] transforms2.LocalRotation2,
        [none] maps.MapArrow
    );
}
