// the realm's quest named name (gam/zoxel/sys/quests.c)
static inline entity realm_quest_named(ecs* world, entity realm, const char* name) {
    entity found = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !found; i++) {
            const entity e = it.entities[i];
            if (zox_has(e, Quest) && zox_has(e, ZoxName) && !strcmp(zox_getv(e, ZoxName), name)) {
                found = e;
            }
        }
    }
    return found;
}

// a speech node saying line after node, returns it
static inline entity link_speech(ecs* world, entity node, const char* line) {
    const entity speech = spawn_node_speech(world, line);
    new_link_single_node(world, node, speech);
    return speech;
}

// a quest's dialogue (its root a QuestDialogue linked to its quest, QuestLink): greets, then
//  - without the quest: leads on, asks, gives it, says it's given
//  - doing it: says it's still to do
//  - done it: says so, rewards, hands it in, says bye; handed in already: says it's done
void spawn_quest_dialogue(ecs* world, entity realm, entity quest, const char* const* lines) {
    const entity root = spawn_node_speech(world, lines[0]);
    zox_add(root, QuestDialogue);
    zox_add(root, Dialogue);
    zox_set_parent(world, root, realm);
    zox_link(world, root, QuestLink, quest);
    const entity has_quest = spawn_node_has_quest(world, quest);
    new_link_single_node(world, root, has_quest);
    // no quest, give it
    const entity leadon = spawn_node_speech(world, lines[1]);
    zox_add(leadon, NodeFalse);
    new_link_single_node(world, has_quest, leadon);
    const entity ask = link_speech(world, leadon, lines[2]);
    const entity give = spawn_node_give_quest(world, quest);
    new_link_single_node(world, ask, give);
    link_speech(world, give, lines[3]);
    // has it, done?
    const entity has_done = spawn_node_has_quest_done(world, quest);
    zox_add(has_done, NodeTrue);
    new_link_single_node(world, has_quest, has_done);
    const entity doing = spawn_node_speech(world, lines[4]);
    zox_add(doing, NodeFalse);
    new_link_single_node(world, has_done, doing);
    // done, handed in?
    const entity has_handedin = spawn_node_has_quest_handedin(world, quest);
    zox_add(has_handedin, NodeTrue);
    new_link_single_node(world, has_done, has_handedin);
    const entity done = spawn_node_speech(world, lines[5]);
    zox_add(done, NodeFalse);
    new_link_single_node(world, has_handedin, done);
    const entity reward = link_speech(world, done, lines[6]);
    const entity handin = spawn_node_handin_quest(world, quest);
    new_link_single_node(world, reward, handin);
    link_speech(world, handin, lines[7]);
    const entity handedin = spawn_node_speech(world, lines[5]);
    zox_add(handedin, NodeTrue);
    new_link_single_node(world, has_handedin, handedin);
}

// each of the realm's slay quests' dialogue
void spawn_quest_dialogues(ecs* world, entity realm) {
    for (int i = 0; i < (int) (sizeof(realm_slay_quests) / sizeof(realm_slay_quests[0])); i++) {
        const entity quest = realm_quest_named(world, realm, realm_slay_quests[i].name);
        if (!zox_valid(quest)) {
            zox_logw("no realm quest [%s] for its dialogue", realm_slay_quests[i].name);
            continue;
        }
        spawn_quest_dialogue(world, realm, quest, realm_slay_quests[i].lines);
    }
}

void dialogue_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_dialoguetrees) {
            continue;
        }
        spawn_quest_dialogues(world, e);
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
                "Helll",
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
    zox_sys_on_end();
} zoxd_system(dialogue_realm_spawn_system);
