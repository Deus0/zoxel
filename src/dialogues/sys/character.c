// Links a npc to a dialogue!
zox_sys2(CharacterDialogueSystem) {
    byte dbg_log = 0;
    uint capacity = 256;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity realm = zox_get_link(world, e, RealmLink);
        byte is_give_quest = zox_has(e, QuestGiver); // rand() % 100 >= 94;
        entity dialogue_type = is_give_quest ?
            zox_id(QuestDialogue) :
            zox_id(Greetings);
        entity trees[capacity];
        uint length = zox_get_children_by_id(
            world,
            realm,
            trees,
            capacity,
            dialogue_type);
        if (!length) {
            zox_loge("No dialogue found on realm [%s]",
                zox_getn(realm));
            continue;
        }
        entity tree = trees[rand() % length];
        zox_link(world, e, Dialogue, tree);
        if (!is_give_quest) {
            continue;
        }
        // TODO: Find the quest, linked in the dialogue nodegraph here
        entity quests[capacity];
        uint quests_length = zox_get_children_by_id(
            world,
            realm,
            quests,
            capacity,
            zox_id(Quest));
        // Add our test quest to our test character
        entity dialogue_quest = quests[rand() % quests_length];
        entity user_quest = spawn_quest_giving(
            world,
            e,
            dialogue_quest);
        if (dbg_log) {
            zox_log("User [%s] has new quest to give [%lu]",
                zox_getn(e),
                user_quest);
        }
    }
} zox_sys_end(CharacterDialogueSystem);
