entity spawn_dialogue_node(
    ecs* world,
    entity prefab,
    char* text)
{
    zox_instance(prefab);
    zox_name("dialogue_node");
    set_DialogueText(world, e, text);
    return e;
}

