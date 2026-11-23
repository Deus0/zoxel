entity spawn_prefab_model_group(ecs* world) {
    zox_prefab();
    zox_prefab_name("model_group");
    zox_add(e, ModelGroup);
    return e;
}

// an animation: a model group whose children are its frames (spawn_blueprint_animation)
entity spawn_prefab_vox_animation(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_animation");
    zox_add(e, VoxAnimation);
    zox_setv(e, AnimationFps, 6);
    return e;
}
