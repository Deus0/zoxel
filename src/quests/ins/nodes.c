entity spawn_node_give_quest(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node_give_quest);
    zox_name("node_give_quest");
    // Links our quest to the node
    zox_link(world, e, QuestLink, quest);
    return e;
}

entity spawn_node_has_quest(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node_has_quest);
    zox_name("node_has_quest");
    // Links our quest to the node
    zox_link(world, e, QuestLink, quest);
    return e;
}

