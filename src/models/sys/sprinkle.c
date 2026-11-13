// Runs sprinkle and speckle nodes from a model process (voctree_sprinkle)
//  - the box scales like paint and fill (to the model's size, then its depth)
//  - a dot is a voxel at the node's NodeDepth (i.e. 4: it stays at Block Res 4's lod), else at the model's
//  - Count is the most dots, each model rolls 2/3 to all of it (variation); at the blueprint's depth, a finer model gets more (as its surface has more voxels), so it looks alike
//  - seeded by the model's seed and the node's place in its graph, so variants differ and redoing one doesn't
void process_node_model_sprinkle(ecs* world, entity node, entity vox, lint seed, byte3 position, byte3 size) {
    const byte value = zox_getv(node, NodeVoxel);
    const byte model_depth = zox_getv(vox, NodeDepth);
    const ColorRGBs* colors = zox_get(vox, ColorRGBs);
    if (!value || value > colors->length) {
        zox_loge("[sprinkle] node [%s] voxel [%i] isn't one of [%i] colors", zox_getn(node), value, colors->length);
        return;
    }
    shift_node_transform(model_depth, &position, &size);
    int count = zox_has(node, Count) ? zox_getv(node, Count) : 1;
    const int shift = (int) model_depth - (int) nodegraph_max_depth;
    if (shift > 0) {
        count <<= 2 * shift;
    } else if (shift < 0) {
        count >>= -2 * shift;
        if (count < 1) {
            count = 1;
        }
    }
    const byte raised = zox_getv(node, NodeType) == zox_model_node_sprinkle;
    const uint node_seed = (uint) seed * 2654435761u + (uint) position.x * 73856093u +
        (uint) position.y * 19349663u + (uint) position.z * 83492791u + value;
    // Count is the most: each model rolls two thirds to all of it, so variants differ in how many too
    const int fewest = (2 * count + 2) / 3;
    count = fewest + (int) (seed_rand(node_seed + 101) % (uint) (count - fewest + 1));
    zox_muter(vox, VoxelNode, voctree);
    spinlock* write_lock = zox_octree_lock(world, vox);
    zox_octree_lock_begin(write_lock);
    // a dot is one voxel at the node's depth (NodeDepth), else one of the model's
    const byte dot_depth = zox_has(node, NodeDepth) && zox_getv(node, NodeDepth) ? zox_getv(node, NodeDepth) : model_depth;
    const int dot = dot_depth < model_depth ? 1 << (model_depth - dot_depth) : 1;
    voctree_sprinkle(voctree, model_depth, value, position, size, count, node_seed, raised, dot);
    zox_octree_lock_end(write_lock);
}

void sprinkle_model_node_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ModelSize, bounds);
        const entity model = zox_get_link(world, e, ModelLink);
        if (!zox_valid(model)) {
            zox_logw("[sprinkle_model_node_system] Node has Invalid Model [%s]", zox_getn(e));
            zox_add(e, TriggerEnd);
            continue;
        }
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node) {
            continue;
        }
        const byte node_type = zox_getv(node, NodeType);
        if (node_type != zox_model_node_sprinkle && node_type != zox_model_node_speckle) {
            continue;
        }
        if (!zox_has(node, NodeVoxel) || !zox_has(node, Shape3Position) || !zox_has(node, Shape3Size)) {
            zox_loge("[sprinkle_model_node_system] Node [%s] has invalid components.", zox_getn(node));
            zox_add(e, TriggerEnd);
            continue;
        }
        if (!zox_has(model, NodeDepth) || !zox_has(model, ColorRGBs) || !zox_has(model, VoxelNode)) {
            zox_loge("[sprinkle_model_node_system] Model [%s] has invalid components.", zox_getn(model));
            zox_add(e, TriggerEnd);
            continue;
        }
        byte3 position = zox_getv(node, Shape3Position);
        byte3 size = zox_getv(node, Shape3Size);
        if (!byte3_equals(bounds->value, byte3_zero)) {
            scale_node_transform(bounds->value, &position, &size);
        }
        const lint seed = zox_has(model, Seed) ? zox_getv(model, Seed) : 0;
        process_node_model_sprinkle(world, node, model, seed, position, size);
        zox_add(e, TriggerEnd);
    }
    zox_sys_on_end();
} zoxd_system(sprinkle_model_node_system);
