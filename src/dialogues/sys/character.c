// a third of villagers give quests, picked by their seed (the same ones after a load)
static inline byte is_quest_giver(ecs* world, entity e) {
    return zox_has(e, Villager) && zox_has(e, Seed) && ((uint64_t) zox_getv(e, Seed) / 7) % 3 == 0;
}

// Links a npc to a dialogue: a quest giver (is_quest_giver, tagged QuestGiver) one of the realm's quest dialogues and
//  that dialogue's quest to give (its QuestLink), the rest a greeting
void character_dialogue_system(iter* it) {
    zox_sys_on_begin();
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
        const entity realm = zox_get_link(world, e, RealmLink);
        const byte is_give_quest = is_quest_giver(world, e);
        entity trees[capacity];
        const uint length = zox_get_children_by_id(world, realm, trees, capacity,
            is_give_quest ? zox_id(QuestDialogue) : zox_id(Greetings));
        if (!length) {
            zox_loge("No dialogue found on realm [%s]", zox_getn(realm));
            continue;
        }
        const entity tree = trees[(uint64_t) zox_getv(e, Seed) % length];
        zox_link(world, e, Dialogue, tree);
        const entity quest = zox_get_link(world, tree, QuestLink);
        if (is_give_quest && zox_valid(quest)) {
            zox_add(e, QuestGiver);
            spawn_quest_giving(world, e, quest);
        }
    }
    zox_sys_on_end();
} zoxd_system(character_dialogue_system);
