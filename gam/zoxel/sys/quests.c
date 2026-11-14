// the realm's slay quests, each its own dialogue (gam/zoxel/sys/dialogues.c), given by some villagers
//  - lines: greet, lead on, ask, given, still doing, done (and after handed in), reward, bye
typedef struct {
    const char* name;
    byte target;
    const char* lines[8];
} realm_slay_quest;

const realm_slay_quest realm_slay_quests[] = {
    { "Slem Infestation", 6, { "Welcome traveler.", "I have a quest you may like.", "I require cookies!",
        "My Cookie Furnace Screams.", "You have already been commanded.", "You did well...", "I shall reward thee.",
        "Go on, giitt." } },
    { "Slem Cull", 10, { "Ah, a strong arm.", "The slems eat my crops.", "Cull ten of them for me.",
        "The fields will thank you.", "The slems still feast.", "The fields are quiet again.",
        "Take this, for your trouble.", "Safe travels." } },
    { "Slem Scouting", 3, { "Psst, over here.", "Slems lurk past the gate.", "Thin them out, just three.",
        "Quietly now.", "Three, I said.", "Neatly done.", "A small token.", "Off you go." } },
};

void quests_realm_spawn_system(iter* it) {
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
        spawn_realm_quest(world, prefab_quest, e, "Find Bob", "discord");
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
        // slay them dirty slems
        for (int j = 0; j < (int) (sizeof(realm_slay_quests) / sizeof(realm_slay_quests[0])); j++) {
            const entity quest = spawn_realm_quest(world, prefab_quest, e, realm_slay_quests[j].name, "slay_slems");
            zox_add(quest, SlayQuest);
            zox_setv(quest, QuestValue, 0);
            zox_setv(quest, QuestTarget, realm_slay_quests[j].target);
            zox_link(world, quest, CharacterLink, slime);
        }
        zox_logv("Realm [quests] [X] spawned.");
    }
    zox_sys_on_end();
} zoxd_system(quests_realm_spawn_system);
