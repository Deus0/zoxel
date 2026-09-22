zox_sys2(QuestsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(CharacterLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(CharacterLinks, characters);
        if (state->value != zox_generate_realm_quests) {
            continue;
        }
        // slay them dirty slems
        {
            spawn_realm_quest(
                world,
                prefab_quest,
                e,
                "Find Bob",
                "discord");
        }
        if (!characters->length) {
            zox_logw("no characters to spawn realm quests with");
            continue;
        }
        {
            entity quest = spawn_realm_quest(
                world,
                prefab_quest,
                e,
                "Slem Infestation",
                "slay_slems");
            // TODO: Objective: Slay 10 Slimes
            zox_add(quest, SlayQuest);
            zox_set(quest, QuestValue, { 0 });
            zox_set(quest, QuestTarget, { 10 });
            entity slime = characters->value[1];
            zox_link(world, quest, CharacterLink, slime);
        }
        zox_logv("Realm [quests] [X] spawned.");
    }
} zox_sys_end(QuestsRealmSpawnSystem);
