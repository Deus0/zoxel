#include "activate.c"
#include "character.c"
#include "select.c"

void zox_define_systems_actions(ecs* world) {
    zox_system(
        ActiveActionSystem,
        EcsOnUpdate,
        [in] actions.ActionIndex,
        [out] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [none] characters.Character,
    );
    zox_system(
        ActionActivateSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionA,
        [in] actions.ActiveAction,
        [none] characters.Character,
    );
    zox_system_1(
        CharacterActionsSpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        // [in] realms.RealmLink,
        // [out] actions.ActionLinks
    );
}
