entity spawn_process_model(
    ecs* world,
    entity node,
    entity model)
{
    if (!zox_valid(model)) { //|| !zox_has(model, Model)) {
        zox_loge("Invalid model [%s] in model process [%s]",
            zox_getn(model),
            zox_getn(node));
        return 0;
    }
    zox_instance(prefab_process);
    zox_name("process_model");
    zox_add(e, ModelRun);
    zox_setv(e, Seed, 0);
    zox_setv(e, ModelSize, byte3_zero);
    zox_link(world, e, NodeLink, node);
    zox_setv(e, ModelLink, model);
    return e;
}
