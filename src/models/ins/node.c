entity spawn_node_model(
    ecs* world,
    entity prefab,
    byte type)
{
    zox_instance(prefab);
    zox_name("node_model");
    if (type) {
        zox_set(e, NodeType, { type });
    }
    return e;
}

// Add Shape3Position and Shape3Size's
entity spawn_node_model_at(
    ecs* world,
    entity prefab,
    byte type,
    byte3 position,
    byte3 size,
    byte voxel)
{
    zox_instance(prefab);
    zox_name("node_modelt");
    zox_set(e, NodeType, { type });
    zox_set(e, Shape3Position, { position });
    zox_set(e, Shape3Size, { size });
    zox_set(e, NodeVoxel, { voxel });
    return e;
}

entity spawn_node_model_size(
    ecs* world,
    float3 min,
    float3 max)
{
    zox_instance(prefab_node_model);
    zox_name("node_model_size");
    zox_setv(e, NodeType, zox_model_node_size);
    float6 bounds = {
        .x = min.x, .y = max.x,
        .z = min.y, .w = max.y,
        .u = min.z, .v = max.z
    };
    zox_setv(e, ModelRatio, bounds);
    return e;
}

entity spawn_node_model_end(ecs* world) {
    return spawn_node_model(
        world,
        prefab_node_model,
        zox_model_node_end);
}


// a sprinkle (dots on the surface) or speckle (spots painted in it) node: count dots of voxel in a box,
//  each a voxel at dot_depth (0: the model's)
//  (position, size: from the corner, blueprint voxels), see voctree_sprinkle
entity spawn_node_model_sprinkle(
    ecs* world,
    byte type,
    byte voxel,
    byte count,
    byte dot_depth,
    byte3 position,
    byte3 size)
{
    entity e = spawn_node_model_at(world, prefab_node_model, type, position, size, voxel);
    zox_name("node_model_sprinkle");
    zox_setv(e, Count, count);
    zox_setv(e, NodeDepth, dot_depth);  // a dot is one voxel at this depth
    return e;
}
