entity spawn_prefab_process_model(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("process_model");
    zox_setv(e, Seed, 0);
    zox_setv(e, ModelLink, 0);
    zox_setv(e, ModelSize, byte3_zero);
    return e;
}