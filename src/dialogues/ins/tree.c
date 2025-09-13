entity spawn_dialogue_tree(
    ecs* world,
    entity prefab,
    char* text
) {
    zox_instance(prefab);
    zox_name("dialogue_tree");
    set_node_speech(world, e, text);
    return e;
}