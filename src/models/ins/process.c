entity spawn_process_model(
    ecs* world,
    entity node,
    entity model)
{
    zox_instance(prefab_process);
    zox_name("process_model");
    zox_add(e, ModelRun);
    zox_setv(e, Seed, 0);
    // zox_setv(e, ModelLink, 0);
    zox_setv(e, ModelSize, byte3_zero);
    zox_link(world, e, NodeLink, node);
    zox_setv(e, ModelLink, model);
    // events
    // zox_setv(e, NodetreeBegin, zox_dirty_trigger);
    return e;
}
