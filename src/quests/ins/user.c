entity spawn_user_quest(ecs* world, entity parent, entity prefab) {
    entity e = zox_ins_named(world, prefab);
    if (parent) {
        zox_set_parent(world, e, parent);
        zox_set(e, UserLink, { parent });
    }
    return e;
}
