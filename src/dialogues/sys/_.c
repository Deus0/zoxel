#include "character.c"
#include "ui.c"
#include "animate.c"
#include "animate_end.c"
#include "sound.c"

void define_systems_dialogues(ecs* world) {
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
        [in] TargetText,
        [in] AnimateTextTimeLimits,
        [out] AnimateTextBegin,
        [out] AnimateTextTime,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [out] GlyphSpawnedDirty,
        [out] AnimateTextEnded
    );
    zox_system(
        AnimateTextEndSystem,
        zoxp_update,
        [in] AnimateTextEnded,
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
        [in] GlyphSpawnedDirty,
        [in] AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
}
