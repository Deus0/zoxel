#include "character.c"
#include "realm.c"
#include "ui.c"

void define_systems_dialogues(ecs* world) {
    zox_system(
        DialogueSpeechSystem,
        EcsOnUpdate,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] DialogueUILink
    );
}