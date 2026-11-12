extern entity get_speaker_a(ecs*, entity);
extern entity get_speaker_b(ecs*, entity);

#include "action.c"
#include "has.c"
#include "slay.c"
#include "marker.c"

void zox_events_quests(ecs* world) {
    // when a node begins (Begin is added each time a process moves to a node, Update only when it wasn't
    //  already there, so a process dirtied again missed its quest nodes: has quest said "You did well...")
    zox_on_add(
        zox_event_give_quest,
        [none] nodes.NodeRun,
        [none] core.Begin,
    );
    zox_on_add(
        zox_event_has_quest,
        [none] nodes.NodeRun,
        [none] core.Begin,
    );
    zox_on_add(
        zox_event_quest_slay,
        [none] characters.Character,
        [none] core.DeathDirty, // Dead,
    );
    zox_on_add(
        zox_event_quest_marker,
        [none] quests.Quest,
        [none] !quests.QuestHandedin,
        [none] core.Dirty,
    );
}
