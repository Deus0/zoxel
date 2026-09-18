#include "character.c"
#include "ui.c"
#include "animate.c"
#include "animate_end.c"
#include "sound.c"

void zox_systems_dialogues(ecs* world) {
    zox_system(
        DialogueSpeechSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [none] dialogues.DialogueProcess
    );
    zox_system(
        AnimateTextSystem,
        zoxp_update,
        [in] dialogues.TargetText,
        [in] dialogues.AnimateTextTimeLimits,
        [out] dialogues.AnimateTextBegin,
        [out] dialogues.AnimateTextTime,
        [out] texts.TextData,
        [out] dialogues.GlyphSpawnedDirty,
        [out] dialogues.AnimateTextEnded,
        [none] texts.Text,
    );
    zox_system(
        AnimateTextEndSystem,
        zoxp_update,
        [in] dialogues.AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
    zox_system_1(
        CharacterDialogueSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
    );
    zox_system_1(
        DialogueSoundSystem,
        zoxp_spawn,
        [in] dialogues.GlyphSpawnedDirty,
        [in] dialogues.AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
}
