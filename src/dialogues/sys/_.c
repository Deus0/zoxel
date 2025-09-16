#include "character.c"
#include "realm.c"
#include "ui.c"
#include "animate.c"

void define_systems_dialogues(ecs* world) {
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
}