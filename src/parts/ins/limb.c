entity spawn_blueprint_flesh_part(
    ecs* world,
    byte3 nsize,
    byte depth,
    color skin_color,
    uint seed)
{
    // each part's skin varies a little from the body's, on purpose (mutations, a simulation's variation)
    //  - clamped: a byte += ±15 wrapped round, a dark channel became ~250 and an arm went olive
    //  - from the seed (the realm's body and the part), the same realm the same skins
    skin_color.r = (byte) int_clamp((int) skin_color.r + seed_range(seed, -15, 15), 0, 255);
    skin_color.g = (byte) int_clamp((int) skin_color.g + seed_range(seed + 1, -15, 15), 0, 255);
    skin_color.b = (byte) int_clamp((int) skin_color.b + seed_range(seed + 2, -15, 15), 0, 255);
    entity e = spawn_node_model_colors(
        world,
        skin_color,
        1);
    byte3 nposition_1 = byte3_half(nsize);
    entity e2 = spawn_node_model_at(
        world,
        prefab_node_model,
        zox_model_node_fill,
        nposition_1,
        nsize,
        1);
    zox_setv(e2, NodeDepth, depth);
    new_link_single_node(world, e, e2);
    // Finish Him!
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, e2, end_node);
    return e;
}
