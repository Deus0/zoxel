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

void define_systems_controllers3(ecs *world) {
    zox_system(
        Player3DMoveSystem,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        PlayerFlySystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system(
        Player3RotateSystem,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        Player3DJumpSystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system_1(
        ActionsShortcutSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        Player3DTriggerSystem,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system_1(
        PlayerToggleCameraSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system_1(
        FreeRoamToggleSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        place_camera_head_system,
        zoxp_update,
        [in] bones.SkeletonDirty,
        // [in] bones.HeadBoneLink,
        [in] blocks.BlockScale,
        [none] bones.Skeleton
    );
    // Shortcuts
    zox_system(
        QolShortcutsSystem,
        zoxp_update,
        [none] players.Player
    );
    // Dialogue
    zox_system_1(
        DialogueBeginSystem,
        zoxp_spawn,
        [in] core.TriggerActionB,
        [in] vrays.RaycastVoxelData,
    );
    // Spawns animation events
    zox_system_1(
        PlayerDialogueSystem,
        zoxp_spawn, // zoxp_update,
        [out] players.PlayerState
    );
    // NOTE: Spawns animation events
    zox_system_1(
        CameraPlayerStateSystem,
        zoxp_spawn, // zoxp_update,
        [in] players.PlayerStateDirty,
        [out] players.PlayerState
    );
    zox_system(
        DialogueExitSystem,
        zoxp_update,
        [in] core.TriggerActionE,
    );
    zox_system(
        DialogueEndSystem,
        zoxp_update,
        [in] nodes.NodetreeEnd,
        [out] dialogues.SpeakerLinks,
        [none] dialogues.DialogueProcess
    );
    zox_system(
        WalkStateSystem,
        zoxp_update,
        [in] physics3.Velocity3D,
        [out] bones.WalkState,
        [none] bones.Skeleton
    );

}
