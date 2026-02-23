// TODO: add a bunch of modifiers, like noise, eyes, etc for tests
/*entity spawn_model_nodegraph_chest(ecs* world, entity p, byte3 size) {

    byte3 core_position = byte3_half(size);

    entity e = spawn_node_model(world, p, zox_model_node_colors);

    entity e2 = spawn_node_model_at(world, p, zox_model_node_fill, core_position, size);
    new_link_single_node(world, e, e2);

    return e;
}
*/
