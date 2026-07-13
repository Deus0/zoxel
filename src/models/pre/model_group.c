entity spawn_prefab_model_group(ecs* world) {
    zox_prefab();
    zox_prefab_name("model_group");
    zox_add_tag(e, ModelGroup);
    return e;
}
