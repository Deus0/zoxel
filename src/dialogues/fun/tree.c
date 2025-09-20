extern entity spawn_dialogue_tree(ecs*, entity, char*);
extern entity spawn_dialogue_leaf(ecs*, entity, char*);

entity spawn_dialogue_tree_texts(
    ecs* world,
    entity prefab_tree,
    entity prefab_leaf,
    char** texts,
    byte count
) {
    if (!count) {
        zox_log_error("Cannot have 0 texts.");
        return 0;
    }
    entity e = spawn_dialogue_tree(
        world,
        prefab_tree,
        texts[0]
    );

    entity last_node = e;
    for (int i = 0; i < count - 1; i++) {
        entity e2 = spawn_dialogue_leaf(
            world,
            prefab_leaf,
            texts[i + 1]
        );

        new_link_single_node(
            world,
            last_node,
            e2
        );
        last_node = e2;
    }
    return e;
}