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
        // TODO: Refactor Tree to Realms Dialogue Gen
        entity dialogue_tree = spawn_dialogue_tree(
            world,
            prefab_dialogue_tree,
            "Well, well, well..."
        );
        entity test_dialogue_leaf = spawn_dialogue_leaf(
            world,
            prefab_dialogue_leaf,
            "Look what the cat dragged in, ay.."
        );
        entity test_dialogue_leaf2 = spawn_dialogue_leaf(
            world,
            prefab_dialogue_leaf,
            "Leave for now! Come back with cookies."
        );

        new_link_single_node(
            world,
            dialogue_tree,
            test_dialogue_leaf
        );

        new_link_single_node(
            world,
            test_dialogue_leaf,
            test_dialogue_leaf2
        );
        add_to_DialoguetreeLinks(&trees, dialogue_tree);
    }


    zox_set_ptr(realm, DialoguetreeLinks, trees);

    zox_logv("At [%f] Realm [dialogues] [%i] spawned.", zox_current_time, trees.length);
}
