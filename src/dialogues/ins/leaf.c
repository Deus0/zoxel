entity spawn_dialogue_leaf(
    ecs* world,
    entity prefab,
    char* text
) {
    zox_instance(prefab);
    zox_name("dialogue_leaf");
    set_node_speech(world, e, text);
    return e;
}