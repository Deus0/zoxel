#include "activate.c"
#include "character.c"
#include "select.c"
#include "hold.c"
#include "raycaster.c"

void zox_systems_actions(ecs* world) {
    zox_system(
        active_action_system,
        zoxp_update,
        [in] actions.ActionIndex,
        [out] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [none] characters.Character,
    );
    zox_system(
        action_activate_system,
        zoxp_update,
        [in] core.TriggerActionA,
        [in] actions.ActiveAction,
        [none] characters.Character,
    );
    zox_system(
        active_action_range_system,
        zoxp_update,
        [in] actions.ActiveActionDirty,
        [in] actions.ActiveAction,
        [out] vrays.RaycastRange,
        [out] vrays.RaycastType,
        [none] characters.Character
    );
    zox_system_1(
        character_actions_spawn_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character
    );
    zox_system_1(
        active_action_hold_system,
        zoxp_spawn,
        [in] actions.ActiveActionDirty,
        [in] actions.ActiveAction,
        [none] bones.SkeletonOwner,
        [none] characters.Character,
    );
}
