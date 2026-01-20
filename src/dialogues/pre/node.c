entity spawn_prefab_dialogue_node(ecs* world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("dialogue_node");
    zox_add_tag(e, DialogueNode);
    zox_prefab_set(e, DialogueText, { });
    return e;
}