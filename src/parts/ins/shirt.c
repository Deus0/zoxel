// a shirt's blueprint, worn over a chest (doc/body_equips.md), in blueprint voxels (corner boxes):
//  - a torso box, margin wider than the chest each side (1 a tight vest, 2 a boxy shirt), its height
//  - sleeves (sleeve voxels out past the torso, over the shoulders, 0 for none), a neck hole cut from its top
//  - trim: a hem, a collar round the neck, cuffs, buttons down its front (+z)
//  - chest_size: the chest blueprint size it fits over (the biggest chest of the realm)
entity spawn_blueprint_shirt(
    ecs* world,
    byte3 chest_size,
    byte depth,
    color cloth_color,
    color trim_color,
    byte margin,
    byte sleeve)
{
    const byte3 torso = { chest_size.x + 2 * margin, chest_size.y, chest_size.z + 2 * margin };
    const byte width = torso.x + 2 * sleeve;        // under 32, the blueprint's size
    const byte sleeve_length = torso.y / 3;
    const byte sleeve_y = torso.y - sleeve_length;
    const byte3 sleeve_size = { sleeve + margin, sleeve_length, torso.z - 2 };
    const byte neck = chest_size.x / 3;
    const byte3 neck_corner = { width / 2 - neck / 2, torso.y - 2, torso.z / 2 - neck / 2 };
    entity nodes[16];
    byte count = 0;
    // cloth: the torso, sleeves, the neck cut (voxel 0)
    nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_box, (byte3) { sleeve, 0, 0 }, torso, 1);
    if (sleeve) {
        nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_box, (byte3) { 0, sleeve_y, 1 }, sleeve_size, 1);
        nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_box,
            (byte3) { width - sleeve_size.x, sleeve_y, 1 }, sleeve_size, 1);
    }
    nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_box, neck_corner, (byte3) { neck, 2, neck }, 0);
    for (byte i = 0; i < count; i++) {
        zox_setv(nodes[i], NodeDepth, depth);
    }
    // trim, paint is a corner and a size (inclusive), over voxels already there
    nodes[count++] = spawn_node_model_colors(world, trim_color, 1);
    nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, byte3_zero, (byte3) { width, 1, torso.z }, 2);
    nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint,
        (byte3) { neck_corner.x - 1, torso.y - 1, neck_corner.z - 1 }, (byte3) { neck + 1, 0, neck + 1 }, 2);
    if (sleeve) {
        nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, (byte3) { 0, sleeve_y, 0 },
            (byte3) { sleeve - 1, 0, torso.z }, 2);
        nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, (byte3) { width - sleeve, sleeve_y, 0 },
            (byte3) { sleeve - 1, 0, torso.z }, 2);
    }
    nodes[count++] = spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, (byte3) { width / 2, 3, torso.z - 1 },
        (byte3) { 0, torso.y - 7, 0 }, 2);
    nodes[count++] = spawn_node_model_end(world);
    // chained after the cloth colour
    entity e = spawn_node_model_colors(world, cloth_color, 1);
    entity previous = e;
    for (byte i = 0; i < count; i++) {
        new_link_single_node(world, previous, nodes[i]);
        previous = nodes[i];
    }
    return e;
}
