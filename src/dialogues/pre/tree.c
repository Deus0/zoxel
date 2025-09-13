entity spawn_prefab_dialogue_tree(
    ecs* world,
    entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_tree");
    zox_add_tag(e, DialogueNode);
    zox_add_tag(e, DialogueTree);
    zox_prefab_set(e, DialogueText, { });
    return e;
}