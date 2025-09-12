entity spawn_prefab_dialogue_leaf(
    ecs* world,
    entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_leaf");
    zox_add_tag(e, DialogueNode);
    zox_add_tag(e, DialogueLeaf);
    return e;
}