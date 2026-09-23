entity spawn_node_model_colors(
    ecs* world,
    color ncolor,
    byte count)
{
    entity e = spawn_node_model(
        world,
        prefab_node_model,
        zox_model_node_colors);
    zox_name("node_model_colors");
    zox_setv(e, Color, ncolor);
    zox_setv(e, NodeColors, count);
    return e;
}

entity spawn_node_model_colors_seed(
    ecs* world,
    byte count)
{
    entity e = spawn_node_model(
        world,
        prefab_node_model,
        zox_model_node_colors);
    zox_name("node_model_colors");
    zox_setv(e, NodeColors, count);
    return e;
}
