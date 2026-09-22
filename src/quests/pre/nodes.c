entity spawn_prefab_node_give_quest(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("node_give_quest");
    zox_add(e, NodeGiveQuest);
    // zox_set(e, NodeBeginEvent, { .count = 0 });
    return e;
}

entity spawn_prefab_node_has_quest(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("node_has_quest");
    zox_add(e, NodeHasQuest);
    return e;
}
