// controller
#include "movement.c"
#include "rotation.c"
#include "jump.c"
#include "trigger.c"
#include "flying.c"
#include "cameras.c"
#include "shortcuts.c"
#include "actions.c"
#include "dialogue_begin.c"
#include "dialogue_player.c"
#include "dialogue_exit.c"
#include "dialogue_end.c"
#include "head_camera.c"
#include "walk.c"
#include "freeroam.c"

void zox_systems_controllers3(ecs *world) {
    zox_system(
        player3d_move_system,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        player_fly_system,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system(
        player3_rotate_system,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        player3d_jump_system,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system_1(
        actions_shortcut_system,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        player3d_trigger_system,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system_1(
        player_toggle_camera_system,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system_1(
        free_roam_toggle_system,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        place_camera_head_system,
        zoxp_update,
        [none] bones.Skeleton,
        [none] bones.SkeletonUpdate,
    );
    // Shortcuts
    zox_system(
        qol_shortcuts_system,
        zoxp_update,
        [none] players.Player
    );
    // Dialogue
    zox_system_1(
        dialogue_begin_system,
        zoxp_spawn,
        [in] core.TriggerActionB,
        [in] vrays.RaycastVoxelData,
    );
    // Spawns animation events
    zox_system_1(
        player_dialogue_system,
        zoxp_spawn, // zoxp_update,
        [out] players.PlayerState
    );
    // NOTE: Spawns animation events
    zox_system_1(
        camera_player_state_system,
        zoxp_spawn, // zoxp_update,
        [in] players.PlayerStateDirty,
        [out] players.PlayerState
    );
    zox_system(
        dialogue_exit_system,
        zoxp_update,
        [in] core.TriggerActionE,
    );
    zox_system_1(
        dialogue_end_system,
        zoxp_spawn,
        [none] nodes.NodeRun,
        [none] dialogues.DialogueProcess,
        [none] core.Exit,
    );
    zox_system(
        walk_state_system,
        zoxp_update,
        [out] bones.WalkState,
        [none] bones.Skeleton
    );

}
