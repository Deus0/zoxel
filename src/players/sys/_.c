#include "state.c"

void zox_define_systems_players(ecs* world) {
    zox_system(
        PlayerStateSystem,
        EcsOnUpdate,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] players.PlayerStateTimer,
        [none] players.Player
    );
}
