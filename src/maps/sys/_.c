#include "spawn.c"
#include "update.c"
#include "arrow.c"
#include "move.c"
#include "view.c"

void zox_systems_maps(ecs* world) {
    zox_system_1(
        map_initialize_system,
        zoxp_spawn,
        [in] maps.MapZoom,
        [in] rendering.Alpha,
        [in] maps.MapPosition,
        [none] maps.Map,
        [none] core.Initialize,
    );
    zox_system_1(
        map_position_system,
        zoxp_spawn,
        [out] maps.MapPosition,
        [none] maps.Map,
        // [none] maps.Minimap
    );
    zox_system_1(
        map_view_system,
        zoxp_update,
        [in] maps.MapBlocksPerTexel,
        [out] maps.MapDrawnAt,
        [out] maps.MapDrawnVersion,
        [out] maps.MapDrawnWhole,
        [out] maps.MapDrawnTime,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] maps.MapView
    );
    zox_system(
        map_arrow_system,
        zoxp_update,
        [out] transforms2.LocalRotation2,
        [none] maps.MapArrow
    );
}
