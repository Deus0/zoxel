#include "leaf.c"
#include "tree.c"
#include "ui.c"

entity get_speaker_a(ecs* world, entity process) {
    return zox_get(process, SpeakerLinks)->value[0];
}

entity get_speaker_b(ecs* world, entity process) {
    return zox_get(process, SpeakerLinks)->value[1];
}
