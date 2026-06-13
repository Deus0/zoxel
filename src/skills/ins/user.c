entity spawn_user_skill(ecs* world, entity parent, entity prefab) {
    entity e = zox_ins_named(world, prefab);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}
