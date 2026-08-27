#include "deactivate.c"
#include "initialize.c"

void zox_systems_core(ecs* world) {
    zox_system(
        initialize_system,
        zoxp_remove,
        [none] core.Initialize,
    );
}
