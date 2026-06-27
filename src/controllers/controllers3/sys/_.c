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

void define_systems_controllers3(ecs *world) {
    zox_system(
        Player3DMoveSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerFlySystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        Player3RotateSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        Player3DJumpSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system_1(
        ActionsShortcutSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system(
        Player3DTriggerSystem,
        EcsPostUpdate,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerToggleCameraSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system(
        HeadCameraSystem,
        EcsPostUpdate,
        [in] bones.SkeletonDirty,
        [in] bones.HeadBoneLink,
        [in] cameras.CameraLink,
        [in] blocks.BlockScale,
        [none] bones.Skeleton
    );
    // Shortcuts
    zox_system(
        QolShortcutsSystem,
        EcsOnUpdate,
        [none] players.Player
    );
    // Dialogue
    zox_system_1(
        DialogueBeginSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionB,
        [in] vrays.RaycastVoxelData,
        [in] players.PlayerLink,
        [out] dialogues.DialogueProcessLink
    );
    zox_system(
        PlayerDialogueSystem,
        EcsOnUpdate,
        [in] characters.CharacterLink,
        [in] layouts2.CanvasLink,
        [out] players.PlayerState
    );
    zox_system(
        DialogueExitSystem,
        EcsOnUpdate,
        [in] triggers.TriggerActionE,
        [in] players.PlayerLink,
        [out] dialogues.DialogueProcessLink
    );
    zox_system(
        DialogueEndSystem,
        EcsOnUpdate,
        [in] nodes.NodetreeEnd,
        [in] dialogues.DialogueUILink,
        [out] dialogues.SpeakerLinks
    );
}
