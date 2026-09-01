extern entity get_speaker_a(ecs*, entity);

void zox_node_give_quest(
    ecs* world,
    entity process,
    entity node)
{
    byte dbg_log = 1;
    if (!zox_valid(node)) {
        zox_loge("[zox_node_give_quest] Invalid [node]");
        return;
    }
    // get speaker of process - FirstSpeaker
    entity quest = zox_get_link(world, node, Quest);
    if (!zox_valid(quest)) {
        zox_loge("[zox_node_give_quest] Invalid [quest]");
        return;
    }
    entity character = get_speaker_a(world, process);
    if (!zox_valid(character)) {
        zox_loge("[zox_node_give_quest] Invalid [speaker_a]");
        return;
    }
    // speaker
    spawn_user_quest(
        world,
        character,
        quest);
    if (dbg_log) {
        zox_log("[zox_node_give_quest] [%s] Given to [%s]",
            zox_getn(quest),
            zox_getn(character));
    }
}

void zox_events_quests(ecs* world) {
    if (!zox_valid(prefab_node_give_quest) ||
        !zox_has(prefab_node_give_quest, NodeBeginEvent))
    {
        zox_loge("[zox_events_quests] Invalid [prefab_node_give_quest]");
        return;
    }
    NodeBeginEvent* event = zox_mut(prefab_node_give_quest, NodeBeginEvent);
    add_to_NodeBeginEvent(event, zox_node_give_quest);
}
