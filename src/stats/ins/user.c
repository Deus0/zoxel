// Stats use name of the parent
entity spawn_stat(ecs* world, entity parent, entity prefab) {
    entity e = zox_ins_named(world, prefab);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    zox_remove_tag(e, RealmStat);
    return e;
}

entity spawn_stat_attribute(ecs* world, entity parent, entity prefab, float value) {
    entity e = spawn_stat(world, parent, prefab);
    zox_set(e, StatValue, { value });
    return e;
}

entity spawn_stat_state(ecs* world, entity parent, entity prefab, float value, float max) {
    entity e = spawn_stat(world, parent, prefab);
    zox_set(e, StatValue, { value });
    zox_set(e, StatValueMax, { max });
    return e;
}

// Add rate later
entity spawn_stat_regen(ecs* world, entity parent, entity prefab, float value) {
    entity e = spawn_stat(world, parent, prefab);
    zox_set(e, StatValue, { value });
    return e;
}

entity spawn_stat_level(ecs* world, entity parent, entity prefab, float value) {
    entity e = spawn_stat(world, parent, prefab);
    zox_set(e, StatValue, { value });
    return e;
}
