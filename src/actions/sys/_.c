#include "activate.c"
#include "character.c"

void define_systems_actions(ecs* world) {
    zox_system(
        ActionActivateSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionA,
        [in] actions.ActionIndex,
        [in] actions.ActionLinks
    );
    zox_system_1(
        CharacterActionsSpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        // [in] realms.RealmLink,
        [out] actions.ActionLinks
    );
}
