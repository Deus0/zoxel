#include "activate.c"
#include "character.c"

void define_systems_actions(ecs* world) {
    zox_system_1(
        ActionActivateSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionB,
        [in] actions.ActionIndex,
        [in] actions.ActionLinks
    );
    zox_system_1(
        CharacterActionsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        // [in] realms.RealmLink,
        [out] actions.ActionLinks
    );
}