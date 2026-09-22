// extern entity get_linked_character(ecs*, entity);

// Has the nodes user got a quest
// for now just use dbg player's character
// NOTE: Running from NodeRunner
void zox_event_has_quest(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // current node
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node || !zox_has(node, NodeHasQuest)) {
            continue;
        }
        // get quest linked to the node
        entity quest = zox_get_link(world, node, QuestLink);
        if (!quest) {
            zox_loge("[zox_event_has_quest] Invalid [quest]");
            continue;
        }
        entity quest_taker = get_speaker_a(world, e);
        if (!quest_taker) {
            zox_loge("Speaker missing in [zox_event_has_quest]");
            continue;
        }
        // if character has quest
        // go through all characters quests
        entity user_quest = get_user_quest(world, quest_taker, quest);
        /*iter it2 = zox_children(world, quest_taker);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity child = it2.entities[j];
                if (!zox_has(child, Quest)) {
                    continue;
                }
                entity prefab = zox_get_prefab(world, child);
                if (prefab == quest) {
                    user_quest = child;
                    break;
                }
            }
        }*/
        byte condition_met = 0;
        if (zox_has(node, NodeHasQuestStarted)) {
            condition_met = zox_valid(user_quest);
        } else if (zox_has(node, NodeHasQuestDone)) {
            condition_met = zox_valid(user_quest) &&
                zox_has(user_quest, QuestDone);
            /*zox_log("   + T [%i/%i]",
             *           zox_getv(user_quest, QuestValue),
             *           zox_getv(user_quest, QuestTarget));*/
        } else if (zox_has(node, NodeHasQuestHandedin)) {
            condition_met = zox_valid(user_quest) &&
                zox_has(user_quest, QuestHandedin);
        } else {
            zox_loge("Unknown QuestNode.");
            continue;
        }
        //temp for now
        // has_quest = quests_count >= 2;
        // TODO: We can set next node here by grabbing all the next nodes and just picking based on the response
        entity next = 0;
        uint node_index = 0;
        while (1) {
            entity possible_node = zox_get_link_index(
                world,
                node,
                NodeLink,
                node_index);
            if (!possible_node) {
                break;
            }
            if (condition_met &&
                zox_has(possible_node, NodeTrue))
            {
                next = possible_node;
                break;
            } else if (!condition_met &&
                zox_has(possible_node, NodeFalse))
            {
                next = possible_node;
                break;
            }
            node_index++;
        }
        if (next) {
            zox_link(world, e, NextNodeLink, next);
        }
        delay_event(world, &delayed_node_end, e, 0.05);
        if (dbg_log) {
            zox_log("[zox_event_has_quest] %s: Nodes [%s -> %s] - Character [%s] [%s] %s Quest [%i]",
                zox_getn(e),
                zox_getn(node),
                zox_getn(next),
                zox_getn(quest_taker),
                zox_getn(user_quest),
                zox_has(node, NodeHasQuestDone) ?
                    "Has Done" :
                    "Has",
                condition_met);
        }
    }
}
