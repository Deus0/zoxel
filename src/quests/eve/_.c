extern entity get_speaker_a(ecs*, entity);
extern entity get_speaker_b(ecs*, entity);

#include "give.c"
#include "has.c"

void zox_events_quests(ecs* world) {
    /*if (!zox_valid(prefab_node_give_quest) ||
        !zox_has(prefab_node_give_quest, NodeBeginEvent))
    {
        zox_loge("[zox_events_quests] Invalid [prefab_node_give_quest]");
        return;
    }*/
    // NodeBeginEvent* event = zox_mut(prefab_node_give_quest, NodeBeginEvent);
    // add_to_NodeBeginEvent(event, zox_node_give_quest);
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
}
