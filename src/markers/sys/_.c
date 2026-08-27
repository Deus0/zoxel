#include "spawn.c"

void zox_define_systems_markers(ecs *world) {
    zox_system_1(
        MarkerSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] ui.ElementLinks
    );
}
