// quest rewards and the find quest
//  - a realm quest rewards QuestRewardCount of its QuestRewardLink item, given as it's handed in
//  - the find quest (FindQuest): talking to anyone inside a town finishes and hands it in (quests_found_town)

// gives a character its quest's reward (its realm quest's, a user quest is an instance of it), the actionbar first
void give_quest_reward(ecs* world, entity character, entity user_quest) {
    const entity quest = zox_get_prefab(world, user_quest);
    const entity item = zox_valid(quest) ? zox_get_link(world, quest, QuestRewardLink) : 0;
    const byte count = zox_valid(quest) && zox_has(quest, QuestRewardCount) ? zox_getv(quest, QuestRewardCount) : 0;
    if (!zox_valid(item) || !count) {
        return;
    }
    if (!add_item_to_slot_manager(world, zox_get_child_by_id(world, character, zox_id(Actionbar)), character, item, count)) {
        add_item_to_slot_manager(world, zox_get_child_by_id(world, character, zox_id(Inventory)), character, item, count);
    }
    // the player's log says so
    char text[96];
    snprintf(text, sizeof(text), "Reward: %i %s", count, zox_has(item, ZoxName) ? zox_getv(item, ZoxName) : "?");
    game_log(world, zox_get_link(world, character, PlayerLink), text);
}

// the player's log: a quest done (handed in)
void log_quest_done(ecs* world, entity character, entity user_quest) {
    char text[96];
    snprintf(text, sizeof(text), "Quest done: %s", zox_has(user_quest, ZoxName) ? zox_getv(user_quest, ZoxName) : "?");
    game_log(world, zox_get_link(world, character, PlayerLink), text);
}

// a character talking to someone in a town: its find quests are done and handed in, rewarded, its log says so
void quests_found_town(ecs* world, entity character) {
    if (!zox_valid(character) || !zox_has(character, InTown)) {
        return;
    }
    iter it = zox_children(world, character);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity user_quest = it.entities[i];
            if (!zox_has(user_quest, Quest) || !zox_has(user_quest, FindQuest) || zox_has(user_quest, QuestHandedin)) {
                continue;
            }
            zox_add(user_quest, QuestDone);
            zox_add(user_quest, QuestHandedin);
            zox_add(user_quest, Dirty);
            log_quest_done(world, character, user_quest);
            give_quest_reward(world, character, user_quest);
            spawn_jingle(world, jingle_quest_handed_in);
        }
    }
}
