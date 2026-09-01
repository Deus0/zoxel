zox_sys2(DialogueRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_dialoguetrees) {
            continue;
        }
        {
            byte texts_count = 4;
            char* texts[] = {
                "Welcome Traveler",
                "I have a quest you may like.",
                "I require cookies!",
                "But I have yet to implement them",
            };
            entity2 dialogue_nodes = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count);
            // Spawn quests here, linked to dialogue
            entity dialogue_quest = spawn_realm_quest(
                world,
                e,
                prefab_quest,
                "Cookies Gett",
                "discord");
            zox_add(dialogue_nodes.x, QuestDialogue);
            // Link quest node to dialogues
            entity give_quest_node = spawn_node_give_quest(
                world,
                dialogue_quest);
            new_link_single_node(world, dialogue_nodes.y, give_quest_node);
        }
        {
            byte texts_count = 3;
            char* texts[] = {
                "Welcome Traveler",
                "Cajeet Greets you.",
                "May the Sun God bless thee.",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
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
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        {
            byte texts_count = 4;
            char* texts[] = {
                "Well, well, well...",
                "Look what the cat dragged in, ay..",
                "Leave me be mortal.",
                "Come back with cookies.",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        {
            byte texts_count = 2;
            char* texts[] = {
                "Hail Penguin",
                "Mr Penguin Watches you.",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        {
            byte texts_count = 3;
            char* texts[] = {
                "I am weary of the black bird",
                "....",
                "We all should be",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        {
            byte texts_count = 7;
            char* texts[] = {
                "Hell",
                "oooo",
                "I...",
                "Go...",
                "...by...",
                "...by",
                "......bye",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        zox_logv("Realm [dialogues] [X] spawned.");
    }
} zox_sys_end(DialogueRealmSpawnSystem);
