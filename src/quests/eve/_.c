extern entity get_speaker_a(ecs*, entity);
extern entity get_speaker_b(ecs*, entity);

#include "action.c"
#include "has.c"
#include "slay.c"

void zox_events_quests(ecs* world) {
    zox_on_add(
        zox_event_give_quest,
        [none] nodes.NodeRun,
        [none] core.Update,
    );
    // TODO: Add observer for has quest
    zox_on_add(
        zox_event_has_quest,
        [none] nodes.NodeRun,
        [none] core.Update,
    );
    zox_on_add(
        zox_event_quest_slay,
        [none] characters.Character,
        [none] core.DeathDirty, // Dead,
    );
}
