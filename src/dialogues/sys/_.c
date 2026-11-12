#include "character.c"
#include "ui.c"
#include "animate_end.c"
#include "sound.c"

void zox_systems_dialogues(ecs* world) {
    zox_system(
        dialogue_speech_system,
        zoxp_update,
        [none] nodes.NodeRun,
        [none] dialogues.DialogueProcess,
        [none] core.Begin,
    );
    zox_system(
        animate_text_end_system,
        zoxp_update,
        [in] texts.AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
    zox_system_1(
        character_dialogue_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
    );
    zox_system_1(
        dialogue_sound_system,
        zoxp_spawn,
        [in] texts.GlyphSpawnedDirty,
        [in] texts.TargetText,
        [in] texts.TextData,
        [out] dialogues.VoiceIndex,
        [none] dialogues.DialogueLabel
    );
}
