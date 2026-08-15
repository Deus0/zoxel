entity spawn_prefab_node_link(ecs *world) {
    zox_prefab();
    zox_prefab_name("nodelink");
    zox_add(e, Nodelink);
    zox_prefab_add(e, NodeParent);
    zox_prefab_add(e, NodeNext);
    return e;
}

entity spawn_node_link(
    ecs *world,
    entity p,
    entity e1,
    entity e2
) {
    zox_instance(p);
    zox_name("nodelink");
    zox_set(e, NodeParent, { e1 });
    zox_set(e, NodeNext, { e2 });
    return e;
}
