#include "deactivate.c"
#include "initialize.c"
#include "update.c"

void zox_systems_core(ecs* world) {
    zox_system(
        pre_initialize_system,
        zoxp_remove, // zoxp_initialize
        [none] core.PreInitialize,
    );
    zox_system(
        initialize_system,
        zoxp_remove, // zoxp_initialize
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_event_system,
        zoxp_spawn,
        [in] core.InitializeEvent,
        [none] core.Initialize,
    );
    zox_system(
        dirty_system,
        zoxp_remove,
        [none] core.Dirty,
    );
    zox_system(
        update_system,
        zoxp_remove,
        [none] core.Update,
    );
}
