entity spawn_prefab_node_link(ecs *world) {
    zox_prefab();
    zox_prefab_name("nodelink");
    zox_add_tag(e, Nodelink);
    zox_prefab_add(e, NodeParent);
    zox_prefab_add(e, NodeNext);
    return e;
}

entity spawn_node_link(
    ecs *world,
    const entity prefab,
    const entity e1,
    const entity e2
) {
    zox_instance(prefab);
    zox_name("nodelink");
    zox_set(e, NodeParent, { e1 });
    zox_set(e, NodeNext, { e2 });
    return e;
}
