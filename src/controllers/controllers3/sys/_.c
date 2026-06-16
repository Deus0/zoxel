// controller
#include "movement.c"
#include "rotation.c"
#include "jump.c"
#include "trigger.c"
#include "respawn.c"
#include "flying.c"
#include "pause.c"
#include "cameras.c"
#include "shortcuts.c"
#include "actions.c"
#include "dialogue_begin.c"
#include "dialogue_player.c"
#include "dialogue_exit.c"
#include "dialogue_end.c"
#include "game_start.c"
#include "game_start2.c"
#include "game_start3.c"
#include "game_start_ui.c"
#include "game_end.c"
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
        Player3RespawnSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerPauseSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] games.GameLink,
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
    // more shortcuts
    zox_system(
        QolShortcutsSystem,
        EcsOnUpdate,
        [none] players.Player
    );
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
    zox_system(
        HeadCameraSystem,
        EcsPostUpdate,
        [in] bones.SkeletonDirty,
        [in] bones.HeadBoneLink,
        [in] cameras.CameraLink,
        [in] blocks.BlockScale,
        [none] bones.Skeleton
    );
    zox_system_1(
        PlayerBeginUISystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        GameStartStreamerSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        PlayerTownFinderSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerBeginSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerGame3EndSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [none] players.Player3
    );
    zox_system(
        WalkStateSystem,
        EcsOnUpdate,
        [in] physics3.Velocity3D,
        [out] bones.WalkState,
        [none] bones.Skeleton
    );
}
