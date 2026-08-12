entity spawn_prefab_connection(ecs *world) {
    zox_prefab();
    zox_prefab_name("connection");
    zox_add_tag(e, Connection);
    zox_prefab_add(e, ConnectionData);
    zox_prefab_set(e, Signal, { 0 });
    zox_prefab_set(e, Transfer, { 0 });
    zox_prefab_set(e, Weight, { 1.0f });
    zox_prefab_set(e, BrainLink, { 0 });
    return e;
}

entity spawn_connection(ecs *world, entity prefab, entity brain, entity a, entity b, float weight) {
    zox_instance(prefab);
    zox_name("connection");
    zox_set(e, ConnectionData, { { a, b } });
    zox_set(e, Weight, { weight });
    zox_set(e, BrainLink, { brain });
    return e;
}
