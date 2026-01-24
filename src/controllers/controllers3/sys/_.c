// controller
#include "movement.c"
#include "rotation.c"
#include "jump.c"
#include "trigger.c"
#include "respawn.c"
#include "flying.c"
// TODO: shortcuts - move to hotkeys or something
#include "pause.c"
#include "cameras.c"
#include "shortcuts.c"
#include "editor.c"
#include "actions.c"

#include "dialogue_begin.c"
#include "dialogue_player.c"
#include "dialogue_exit.c"
#include "dialogue_end.c"

#include "game_start.c"
#include "game_end.c"

void define_systems_controllers3D(ecs *world) {

    zox_system(
        Player3DMoveSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerFlySystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );

    zox_system(
        Player3RotateSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        Player3DJumpSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        Player3DTriggerSystem,
        EcsPostUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system_1(
        Player3RespawnSystem,
        EcsOnUpdate,
        [out] players.PlayerState,
        [out] players.PlayerRespawn,
        [out] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerPauseSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] inputs.DeviceLinks,
        [in] games.GameLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerToggleCameraSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] inputs.DeviceLinks,
        [in] characters.CharacterLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );

    // more shortcuts
    zox_system(
        QolShortcutsSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [none] players.Player
    );
    zox_system_1(
        EditorInputSystem,
        EcsPreStore,
        [in] inputs.DeviceLinks,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system_1(
        ActionsShortcutSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
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

    zox_system_1(
        PlayerGame3StartSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] games.GameLink,
        [in] cameras.CameraLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerGame3EndSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [none] players.Player3
    );
}