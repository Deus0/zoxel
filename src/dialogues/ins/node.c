entity spawn_node_speech(
    ecs* world,
    char* text)
{
    zox_instance(prefab_node_speech);
    zox_name("dialogue_node");
    set_DialogueText(world, e, text);
    return e;
}

