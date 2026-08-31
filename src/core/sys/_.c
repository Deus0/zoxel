#include "deactivate.c"
#include "initialize.c"

void zox_systems_core(ecs* world) {
    zox_system(
        pre_initialize_system,
        zoxp_initialize, // zoxp_remove,
        [none] core.PreInitialize,
    );
    zox_system(
        initialize_system,
        zoxp_initialize, // zoxp_remove,
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_event_system,
        zoxp_spawn,
        [in] core.InitializeEvent,
        [none] core.Initialize,
    );
}
