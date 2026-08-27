#include "deactivate.c"
#include "initialize.c"

void zox_systems_core(ecs* world) {
    zox_system(
        pre_initialize_system,
        zoxp_update, // zoxp_remove,
        [none] core.PreInitialize,
    );
    zox_system(
        initialize_system,
        zoxp_update, // zoxp_remove,
        [none] core.Initialize,
    );
}
