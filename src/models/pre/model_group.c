entity spawn_prefab_model_group(ecs* world) {
    zox_prefab();
    zox_prefab_name("model_group");
    zox_add(e, ModelGroup);
    return e;
}
