#include "player.c"
#include "spawn.c"
#include "update.c"
#include "arrow.c"
#include "move.c"

void zox_define_systems_maps(ecs* world) {
    zox_system_1(
        MapInitializeSystem,
        zoxp_spawn,
        [in] maps.MapZoom,
        [in] rendering.Alpha,
        [in] players.PlayerLink,
        [in] maps.MapPosition,
        [none] core.Initialize,
        [none] maps.Map
    );
    zox_system_1(
        MapPositionSystem,
        zoxp_spawn,
        [in] players.PlayerLink,
        [out] maps.MapPosition,
        [none] maps.Map
        // [none] maps.Minimap
    );
    zox_system(
        MapArrowSystem,
        zoxp_update,
        [in] players.PlayerLink,
        [out] transforms2.LocalRotation2,
        [none] maps.MapArrow
    );
}

void zox_events_maps(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_minimaps);
}
