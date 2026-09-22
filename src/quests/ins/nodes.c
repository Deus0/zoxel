entity spawn_node_give_quest(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node);
    zox_name("node_give_quest");
    zox_add(e, NodeActionQuest);
    zox_add(e, NodeGiveQuest);
    zox_link(world, e, QuestLink, quest);
    return e;
}

entity spawn_node_handin_quest(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node);
    zox_name("node_give_quest");
    zox_add(e, NodeActionQuest);
    zox_add(e, NodeHandinQuest);
    zox_link(world, e, QuestLink, quest);
    return e;
}

entity spawn_node_has_quest(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node);
    zox_name("node_has_quest");
    zox_add(e, NodeHasQuest);
    zox_add(e, NodeHasQuestStarted);
    // Links our quest to the node
    zox_link(world, e, QuestLink, quest);
    return e;
}


entity spawn_node_has_quest_done(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node);
    zox_name("node_has_quest");
    zox_add(e, NodeHasQuest);
    zox_add(e, NodeHasQuestDone);
    // Links our quest to the node
    zox_link(world, e, QuestLink, quest);
    return e;
}


entity spawn_node_has_quest_handedin(
    ecs* world,
    entity quest)
{
    zox_instance(prefab_node);
    zox_name("node_has_quest");
    zox_add(e, NodeHasQuest);
    zox_add(e, NodeHasQuestHandedin);
    zox_link(world, e, QuestLink, quest);
    return e;
}
