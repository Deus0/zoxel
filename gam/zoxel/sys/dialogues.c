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
            entity dialogue = spawn_dialogue_tree_texts(
                world,
                prefab_dialogue_node,
                e,
                texts,
                texts_count);
            zox_add(dialogue, QuestDialogue);
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
                texts_count);
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
                texts_count);
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
                texts_count);
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
                texts_count);
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
                texts_count);
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
                texts_count);
            zox_add(dialogue, Greetings);
        }
        zox_logv("Realm [dialogues] [X] spawned.");
    }
} zox_sys_end(DialogueRealmSpawnSystem);
