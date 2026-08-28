// NOTE: A profile stores the data for a plot graph.
entity spawn_prefab_profile(ecs* world) {
    zox_prefab();
    zox_prefab_name("profile");
    zox_add(e, DoubleData);
    return e;
}

entity spawn_profile(
    ecs* world,
    uint count)
{
    entity e = zox_ins(world, prefab_profile);
    DoubleData data;
    initialize_DoubleData(&data, count);
    zox_set_ptr(e, DoubleData, data);
    return e;
}