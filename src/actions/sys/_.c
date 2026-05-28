#include "activate.c"
#include "character.c"
#include "select.c"
#include "hold.c"

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
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        // [in] realms.RealmLink,
        // [out] actions.ActionLinks
    );
    zox_system_1(
        ActiveActionHoldSystem,
        zoxp_mainthread,
        [in] actions.ActiveActionDirty,
        [in] actions.ActiveAction,
        [in] bones.HandBoneLink,
        [out] bones.RaiseShoulder,
        [none] characters.Character,
    );
}
