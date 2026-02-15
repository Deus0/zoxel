#include "player.c"

void define_systems_bodys(ecs* world) {
    zox_system(
        PlayerBodySpawnSystem,
        EcsOnUpdate,
        [in] core.EntityInitialize,
        [in] rendering.RenderDepth,
        [none] players.PlayerCharacter
    );
}
