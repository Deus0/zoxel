entity spawn_dialogue_node(ecs* world, entity p, char* text) {
    zox_instance(p);
    zox_name("dialogue_node");
    set_node_speech(world, e, text);
    return e;
}