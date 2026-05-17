zox_sys2(QuestsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(CharacterLinks);
    zox_sys_out(QuestLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(CharacterLinks, characters);
        zox_sys_o(QuestLinks, quests);
        if (state->value != zox_generate_realm_quests) {
            continue;
        }
        // slay them dirty slems
        {
            entity quest = spawn_realm_quest(world, e, prefab_quest, "find bob", "discord");
            add_to_QuestLinks(quests, quest);
        }
        if (!characters->length) {
            zox_logw("no characters to spawn realm quests with");
            continue;
        }
        {
            entity quest = spawn_realm_quest(world, e, prefab_quest, "Slem Infestation", "taskbar_lore");
            // TODO: Objective: Slay 10 Slimes
            zox_add_tag(quest, SlayQuest);
            zox_set(quest, QuestValue, { 0 });
            zox_set(quest, QuestTarget, { 10 });
            entity slime = characters->value[0];
            zox_set(quest, CharacterLink, { slime });
            add_to_QuestLinks(quests, quest);
        }
        zox_logv("At [%f] Realm [quests] [%i] spawned.", zox_current_time, quests->length);
    }
} zox_sys_end(QuestsRealmSpawnSystem);
