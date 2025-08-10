
// t for terrain
VoxelNode* set_voxelc(
    VoxelNode* node,
    byte target,
    byte3 position,
    byte value,
    byte depth
) {
    byte depth_reached = depth == target;
    if (!depth_reached && is_closed_VoxelNode(node)) {
        open_VoxelNode(node);
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            kids[i].value = 0;
        }
    }
    // wait this overrides child nodes, rather than reevaluating them
    if (depth_reached) {
        node->value = value;
    }
    if (depth_reached || !has_children_VoxelNode(node)) {
        return node;
    }

    const byte dividor = powers_of_two_byte[target - depth - 1];
    if (dividor == 0) {
        return node; // no need to dive then, we just set voxel anyway
    }
    //const byte3 node_position = position;
    //byte3_modulus_byte(&node_position, dividor);
    byte3 node_position = (byte3) {
        position.x / dividor,
        position.y / dividor,
        position.z / dividor
    };
    byte3_modulus_byte(&position, dividor);

    const byte i = byte3_octree_array_index(node_position);
    if (i >= 8) {
        zox_log_error("[c] node index out of bounds: %i", i);
        return node;
    }

    VoxelNode* kids = get_children_VoxelNode(node);
    node = &kids[i];
    depth++;

    return set_voxelc(
        node,
        target,
        position,
        value,
        depth);
}