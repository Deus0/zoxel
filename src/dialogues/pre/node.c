entity spawn_prefab_dialogue_node(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_node");
    zox_add(e, DialogueNode);
    zox_set(e, DialogueText, { });
    return e;
}
