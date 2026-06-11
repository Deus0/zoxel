zox_sys2(DialogueRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(DialoguetreeLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(DialoguetreeLinks, dialogues);
        if (state->value != zox_generate_realm_dialoguetrees) {
            continue;
        }
        {
            byte texts_count = 3;
            char* texts[] = {
                "Welcome Traveler",
                "Cajeet Greets you.",
                "May the Sun God bless thee.",
            };
            entity e2 = spawn_dialogue_tree_texts(world, prefab_dialogue_node, e, texts, texts_count);
            add_to_DialoguetreeLinks(dialogues, e2);
        }
        {
            byte texts_count = 5;
            char* texts[] = {
                "Greetings traveler..",
                "...",
                "....",
                "...",
                "Well.. get going then."
            };
            entity e2 = spawn_dialogue_tree_texts(world, prefab_dialogue_node, e, texts, texts_count);
            add_to_DialoguetreeLinks(dialogues, e2);
        }
        {
            byte texts_count = 4;
            char* texts[] = {
                "Well, well, well...",
                "Look what the cat dragged in, ay..",
                "Leave me be mortal.",
                "Come back with cookies.",
            };
            entity e2 = spawn_dialogue_tree_texts(world, prefab_dialogue_node, e, texts, texts_count);
            add_to_DialoguetreeLinks(dialogues, e2);
        }
        // zox_set_ptr(realm, DialoguetreeLinks, trees);
        zox_logv("At [%f] Realm [dialogues] [%i] spawned.", zox_current_time, dialogues->length);
    }
} zox_sys_end(DialogueRealmSpawnSystem);
