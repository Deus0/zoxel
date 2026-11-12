void QuestsRealmSpawnSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
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
        entity slime = 0;
        zox_each_link(world, e, CharacterLinks, character) {
            if (zox_has(character, CharacterSlime)) {
                slime = character;
                break;
            }
        }
        if (!slime) {
            zox_logw("no slime character to spawn realm quests with");
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
            zox_setv(quest, QuestValue, 0);
            zox_setv(quest, QuestTarget, 6);
            zox_link(world, quest, CharacterLink, slime);
        }
        zox_logv("Realm [quests] [X] spawned.");
    }
    zox_sys_on_end();
} zoxd_system(QuestsRealmSpawnSystem);
