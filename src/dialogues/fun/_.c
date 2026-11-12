#include "leaf.c"
#include "tree.c"
#include "ui.c"
#include "voice.c"

// Speaker A is the player speaker, otherwise the first speaker found
entity get_speaker_a(ecs* world, entity process) {
    entity first = 0;
    zox_each_link(world, process, SpeakerLinks, speaker) {
        if (zox_valid(zox_get_link(world, speaker, PlayerLink))) {
            return speaker;
        }
        if (!first) {
            first = speaker;
        }
    }
    return first;
}

// Speaker B is the first speaker that isn't Speaker A
entity get_speaker_b(ecs* world, entity process) {
    const entity speaker_a = get_speaker_a(world, process);
    zox_each_link(world, process, SpeakerLinks, speaker) {
        if (speaker != speaker_a) {
            return speaker;
        }
    }
    return 0;
}
