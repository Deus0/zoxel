#include "activate.c"

void define_systems_actions(ecs* world) {
    zox_system_1(
        ActionActivateSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionB,
        [in] actions.ActionIndex,
        [in] actions.ActionLinks
    );
}