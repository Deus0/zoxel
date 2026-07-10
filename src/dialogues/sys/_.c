#include "character.c"
#include "ui.c"
#include "animate.c"
#include "animate_end.c"
#include "sound.c"
realm_clear_system(DialoguetreeLinks);

void define_systems_dialogues(ecs* world) {
    realm_clear_systemd(dialogues, DialoguetreeLinks);
    zox_system(
        DialogueSpeechSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] dialogues.DialogueUILink
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
        [out] ZigelSpawnedDirty,
        [out] AnimateTextEnded
    );
    zox_system(
        AnimateTextEndSystem,
        zoxp_update,
        [in] AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
    zox_system(
        CharacterDialogueSystem,
        zoxp_update,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] dialogues.DialoguetreeLink
    );
    zox_system_1(
        DialogueSoundSystem,
        zoxp_mainthread,
        [in] ZigelSpawnedDirty,
        [in] AnimateTextEnded,
        [none] dialogues.DialogueLabel
    );
}
