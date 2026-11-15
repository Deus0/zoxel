#include "activate.c"
#include "character.c"
#include "select.c"
#include "hold.c"
#include "raycaster.c"
#include "placement_gizmo.c"

void zox_systems_actions(ecs* world) {
    // main thread, after raycast_gizmo_system (vrays, same phase, registered first): it resizes the gizmo it placed
    zox_system_1(
        placement_gizmo_system,
        zoxp_spawn,
        [in] vrays.RaycastVoxelData,
        [in] vrays.RaycastType,
        [in] actions.ActiveAction,
        [none] characters.Character
    );
    zox_system(
        active_action_system,
        zoxp_update,
        [in] actions.ActionIndex,
        [out] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [none] characters.Character,
    );
    zox_system(
        active_action_restore_system,
        zoxp_update,
        [in] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [none] bones.SkeletonOwner,
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
