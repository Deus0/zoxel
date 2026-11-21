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
    zox_on_add(
        animate_text_end_event,
        [none] dialogues.DialogueLabel,
        [none] texts.AnimateTextEnded
    );
    zox_system_1(
        character_dialogue_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
    );
    zox_on_add(
        dialogue_sound_event,
        [in] texts.TargetText,
        [in] texts.TextData,
        [out] dialogues.VoiceIndex,
        [none] dialogues.DialogueLabel,
        [none] texts.GlyphSpawnedDirty
    );
}
