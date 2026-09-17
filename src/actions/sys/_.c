#include "activate.c"
#include "character.c"
#include "select.c"
#include "hold.c"
#include "raycaster.c"

void zox_define_systems_actions(ecs* world) {
    zox_system(
        ActiveActionSystem,
        zoxp_update,
        [in] actions.ActionIndex,
        [out] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [none] characters.Character,
    );
    zox_system(
        ActionActivateSystem,
        zoxp_update,
        [in] core.TriggerActionA,
        [in] actions.ActiveAction,
        [none] characters.Character,
    );
    zox_system(
        ActiveActionRangeSystem,
        zoxp_update,
        [in] actions.ActiveActionDirty,
        [in] actions.ActiveAction,
        [out] vrays.RaycastRange,
        [out] vrays.RaycastType,
        [none] characters.Character
    );
    zox_system_1(
        CharacterActionsSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character
    );
    zox_system_1(
        ActiveActionHoldSystem,
        zoxp_spawn,
        [in] actions.ActiveActionDirty,
        [in] actions.ActiveAction,
        [out] bones.RaiseShoulder,
        [none] characters.Character,
    );
}
