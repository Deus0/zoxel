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
            entity dialogue_quest = zox_get_child_by_id(world, e, zox_id(SlayQuest));
            /*entity dialogue_quest = spawn_realm_quest(
                world,
                prefab_quest,
                e,
                "Cookies Gett",
                "discord");*/
            entity root = spawn_node_speech(world, "Welcome Traveler");
            zox_add(root, QuestDialogue);
            zox_add(root, Dialogue);
            zox_set_parent(world, root, e);

            entity has_quest = spawn_node_has_quest(world, dialogue_quest);
            zox_link(world, root, NodeLink, has_quest);

            entity leadon = spawn_node_speech(world, "I have a quest you may like.");
            zox_add(leadon, NodeFalse);
            zox_link(world, has_quest, NodeLink, leadon);
            entity demand = spawn_node_speech(world, "I require cookies!");
            zox_link(world, leadon, NodeLink, demand);

            entity give_quest = spawn_node_give_quest(world, dialogue_quest);
            zox_link(world, demand, NodeLink, give_quest);
            entity give_quest_speech = spawn_node_speech(world, "My Cookie Furnace Screams.");
            zox_link(world, give_quest, NodeLink, give_quest_speech);
            // Waiting on quest
            entity has_quest_speech = spawn_node_speech(world, "You have already been commanded.");
            zox_add(has_quest_speech, NodeTrue);
            zox_link(world, has_quest, NodeLink, has_quest_speech);
            // Spawn conditional node here
            // If they have quest, do A, if not do B
            // Spawn quests here, linked to dialogue
            // Link quest node to dialogues
            // "But I have yet to implement them",
            /*entity2 dialogue_nodes = spawn_dialogue_tree_texts(
                world,
                e,
                texts,
                texts_count);*/
        }
        {
            byte texts_count = 2;
            char* texts[] = {
                "...",
                "... I was not programmed to be here",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        {
            byte texts_count = 3;
            char* texts[] = {
                "Have you considered buying Zoxel?",
                "...",
                "...I am literally already here",
            };
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
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
                e,
                texts,
                texts_count).x;
            zox_add(dialogue, Greetings);
        }
        zox_logv("Realm [dialogues] [X] spawned.");
    }
} zox_sys_end(DialogueRealmSpawnSystem);
