// a shirt's blueprint, worn over a chest (doc/body_equips.md): the chest blueprint's two blobs, a voxel
//  bigger each side so it covers it, a hem painted at its bottom and a button strip down its front
//  - chest_size: the chest blueprint size it fits over (the biggest chest of the realm)
entity spawn_blueprint_shirt(
    ecs* world,
    byte3 chest_size,
    byte depth,
    color cloth_color,
    color trim_color)
{
    const byte margin = 1;
    const byte3 size = { chest_size.x + 2 * margin, chest_size.y, chest_size.z + 2 * margin };
    entity e = spawn_node_model_colors(world, cloth_color, 1);
    // the body: the chest's pillar, wider
    const byte3 body_size = { (4 * chest_size.x) / 5 + 2 * margin, chest_size.y, (4 * chest_size.z) / 5 + 2 * margin };
    const byte3 body_position = { size.x / 2, body_size.y / 2, size.z / 2 };
    // the shoulders: the chest's upper blob, wider
    const byte3 top_size = { size.x, 1 + chest_size.y / 4, size.z };
    const byte3 top_position = { size.x / 2, ((7 * chest_size.y) / 8) - top_size.y / 2, size.z / 2 };
    entity body = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, body_position, body_size, 1);
    zox_setv(body, NodeDepth, depth);
    new_link_single_node(world, e, body);
    entity top = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, top_position, top_size, 1);
    zox_setv(top, NodeDepth, depth);
    new_link_single_node(world, body, top);
    // trim: paint is a corner and a size, over voxels already there
    entity trim = spawn_node_model_colors(world, trim_color, 1);
    new_link_single_node(world, top, trim);
    entity hem = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, byte3_zero,
        (byte3) { size.x, 1, size.z }, 2);
    new_link_single_node(world, trim, hem);
    entity buttons = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint,
        (byte3) { size.x / 2, 0, size.z / 2 }, (byte3) { 0, size.y, size.z / 2 }, 2);
    new_link_single_node(world, hem, buttons);
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, buttons, end_node);
    return e;
}
