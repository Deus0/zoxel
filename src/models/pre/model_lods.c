// A group of models, based on lods
entity spawn_prefab_model_lods(ecs* world) {
    zox_prefab();
    zox_prefab_name("model_lods");
    zox_setv(e, Seed, 0);
    zox_setv(e, MaxRenderDepth, 0);
    zox_setv(e, ModelLods);
    // NOTE: We should probably just create colors on the model_lods base, and pass down to models
    return e;
}