entity spawn_dialogue_run(
    ecs* world,
    entity prefab,
    entity tree
) {
    zox_instance(prefab);
    zox_name("dialogue_run");

    zox_set(e, NodetreeLink, { tree });

    // Start the run!
    zox_set(e, NodetreeBegin, { zox_dirty_trigger });

    return e;
}