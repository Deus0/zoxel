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
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerFlySystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        Player3RotateSystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        Player3DJumpSystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system_1(
        ActionsShortcutSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        Player3DTriggerSystem,
        zoxp_update, // EcsPostUpdate,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerToggleCameraSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system_1(
        FreeRoamToggleSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        HeadCameraSystem,
        zoxp_update, // EcsPostUpdate,
        [in] bones.SkeletonDirty,
        [in] bones.HeadBoneLink,
        [in] cameras.CameraLink,
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
        zoxp_mainthread,
        [in] triggers.TriggerActionB,
        [in] vrays.RaycastVoxelData,
        [in] players.PlayerLink,
        [out] dialogues.DialogueProcessLink
    );
    // Spawns animation events
    zox_system_1(
        PlayerDialogueSystem,
        zoxp_mainthread, // zoxp_update,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [in] layouts2.CanvasLink,
        [out] players.PlayerState
    );
    zox_system(
        DialogueExitSystem,
        zoxp_update,
        [in] triggers.TriggerActionE,
        [in] players.PlayerLink,
        [out] dialogues.DialogueProcessLink
    );
    zox_system(
        DialogueEndSystem,
        zoxp_update,
        [in] nodes.NodetreeEnd,
        [in] dialogues.DialogueUILink,
        [out] dialogues.SpeakerLinks
    );
    zox_system(
        WalkStateSystem,
        zoxp_update,
        [in] physics3.Velocity3D,
        [out] bones.WalkState,
        [none] bones.Skeleton
    );

}
