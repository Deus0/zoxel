#include "character.c"
#include "ui.c"
#include "animate_end.c"
#include "sound.c"

void zox_systems_dialogues(ecs* world) {
    zox_system(
        DialogueSpeechSystem,
        zoxp_update,
        [none] nodes.NodeRun,
        [none] dialogues.DialogueProcess,
        [none] core.Begin,
    );
    zox_system(
        AnimateTextEndSystem,
        zoxp_update,
        [in] texts.AnimateTextEnded,
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
        [in] texts.GlyphSpawnedDirty,
        [in] texts.AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
}
