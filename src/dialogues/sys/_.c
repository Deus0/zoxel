#include "character.c"
#include "ui.c"
#include "animate.c"
realm_clear_system(DialoguetreeLinks);

void define_systems_dialogues(ecs* world) {
    realm_clear_systemd(dialogues, DialoguetreeLinks);

    zox_system(
        DialogueSpeechSystem,
        EcsOnUpdate,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] DialogueUILink
    );

    zox_system(
        AnimateText2System,
        EcsOnUpdate,
        [in] AnimateTextBegin,
        [in] AnimateTextTime,
        [in] TargetText,
        [out] texts.TextData,
        [out] texts.TextDirty
    );

    zox_system(
        CharacterDialogueSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] dialogues.DialoguetreeLink
    );
}
