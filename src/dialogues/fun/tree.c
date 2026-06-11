// extern entity spawn_dialogue_tree(ecs*, entity, char*);
extern entity spawn_dialogue_node(ecs*, entity, char*);

entity spawn_dialogue_tree_texts(ecs* world, entity prefab, entity parent, char** texts, byte count) {
    if (!count) {
        zox_log_error("Cannot have 0 texts.");
        return 0;
    }
    entity e = 0;
    entity last_node = e;
    for (int i = 0; i < count; i++) {
        entity e2 = spawn_dialogue_node(world, prefab, texts[i]);
        if (!e) {
            // if first node
            e = e2;
        } else {
            new_link_single_node(world, last_node, e2);
        }
        last_node = e2;
    }
    zox_set_parent(world, e, parent);
    return e;
}
