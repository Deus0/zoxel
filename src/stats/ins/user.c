// Stats use name of the parent
entity spawn_stat(
    ecs* world,
    entity prefab,
    entity parent)
{
    entity e = zox_ins_named(world, prefab);
    // zox_remove(e, RealmStat);
    zox_set_parent(
        world,
        e,
        parent);
    return e;
}

entity spawn_stat_attribute(
    ecs* world,
    entity parent,
    entity prefab,
    float value)
{
    entity e = spawn_stat(
        world,
        prefab,
        parent);
    zox_setv(e, StatValue, value);
    return e;
}

entity spawn_stat_state(
    ecs* world,
    entity parent,
    entity prefab,
    float value,
    float max)
{
    entity e = spawn_stat(
        world,
        prefab,
        parent);
    zox_setv(e, StatValue, value);
    zox_setv(e, StatValueMax, max);
    return e;
}

// Add rate later
entity spawn_stat_regen(
    ecs* world,
    entity parent,
    entity prefab,
    float value)
{
    entity e = spawn_stat(
        world,
        prefab,
        parent);
    zox_setv(e, StatValue, value);
    return e;
}

entity spawn_stat_level(
    ecs* world,
    entity parent,
    entity prefab,
    float value)
{
    entity e = spawn_stat(
        world,
        prefab,
        parent);
    zox_setv(e, StatValue, value);
    return e;
}
