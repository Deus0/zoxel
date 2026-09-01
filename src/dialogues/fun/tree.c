// extern entity spawn_dialogue_tree(ecs*, entity, char*);
extern entity spawn_dialogue_node(ecs*, entity, char*);

entity2 spawn_dialogue_tree_texts(
    ecs* world,
    entity prefab,
    entity parent,
    char** texts,
    byte count)
{
    if (!count) {
        zox_loge("[spawn_dialogue_tree_texts] Invalid [texts]");
        return (entity2) { };
    }
    entity first_node = 0;
    entity last_node = 0;
    for (int i = 0; i < count; i++) {
        entity e2 = spawn_dialogue_node(
            world,
            prefab,
            texts[i]);
        if (!first_node) {
            // if first node
            first_node = e2;
        } else {
            new_link_single_node(world, last_node, e2);
        }
        last_node = e2;
    }
    zox_add(first_node, Dialogue);
    zox_set_parent(world, first_node, parent);
    return (entity2) {
        first_node,
        last_node
    };
}
