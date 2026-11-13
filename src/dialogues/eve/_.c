#include "look.c"
// TODO: Add dialogue condition node

void zox_events_dialogues(ecs* world) {
    zox_observe(on_removed_dialogue_process, EcsOnRemove, [none] dialogues.DialogueProcess);
}
