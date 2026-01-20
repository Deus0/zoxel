#include "character.c"
#include "realm.c"
#include "ui.c"
#include "animate.c"
zox_declare_system_state_event(RealmDialoguetrees, GenerateRealm, zox_generate_realm_dialoguetrees, spawn_realm_dialoguetrees)

void define_systems_dialogues(ecs* world) {
    zox_define_system_state_event_1(RealmDialoguetrees, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);

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