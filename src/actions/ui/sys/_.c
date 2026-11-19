#include "castbar.c"

void zox_systems_actions_ui(ecs* world) {
    // main thread: it sets text
    zox_system_1(
        castbar_system,
        zoxp_spawn,
        [out] CastAction,
        [out] ui.BarLevel,
        [none] Castbar
    );
}
