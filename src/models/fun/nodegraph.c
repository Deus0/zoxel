// TODO: add a bunch of modifiers, like noise, eyes, etc for tests
entity spawn_model_nodegraph(ecs* world, entity p, byte type, byte type2) {
    entity e = spawn_node_model(world, p, type);

    entity e2 = spawn_node_model(world, p, type2);

    new_link_single_node(world, e, e2);

    return e;
}