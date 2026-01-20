void spawn_realm_dialoguetrees(ecs *world, const entity realm) {

    zox_geter(realm, DialoguetreeLinks, old);
    if (old) {
        for (int i = 0; i < old->length; i++) {
            if (zox_valid(old->value[i])) {
                zox_delete(old->value[i]);
            }
        }
    }

    DialoguetreeLinks trees = (DialoguetreeLinks) { 0 };

    {
        byte texts_count = 3;
        char* texts[] = {
            "Greetings traveler..",
            "... *stares*",
            "Well, get going then."
        };
        entity e2 = spawn_dialogue_tree_texts(
            world,
            prefab_dialogue_node,
            texts,
            texts_count
        );
        add_to_DialoguetreeLinks(&trees, e2);
    }

    {
        byte texts_count = 3;
        char* texts[] = {
            "Well, well, well...",
            "Look what the cat dragged in, ay..",
            "Leave for now! Come back with cookies."
        };
        entity e2 = spawn_dialogue_tree_texts(
            world,
            prefab_dialogue_node,
            texts,
            texts_count
        );
        add_to_DialoguetreeLinks(&trees, e2);
    }


    zox_set_ptr(realm, DialoguetreeLinks, trees);

    zox_logv("At [%f] Realm [dialogues] [%i] spawned.", zox_current_time, trees.length);
}
