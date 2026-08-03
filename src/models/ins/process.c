entity spawn_process_model(ecs* world, entity prefab, entity node, entity model) {
    zox_instance(prefab);
    zox_name("process_model");
    zox_set(e, NodeStartLink, { node });
    zox_set(e, NodetreeBegin, { zox_dirty_trigger });
    zox_set(e, ModelLink, { model });
    return e;
}