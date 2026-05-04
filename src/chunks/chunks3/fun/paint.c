VoxelNode* fill_voctree(VoxelNode* node, byte target, byte3 position, byte value, byte depth) {

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

    byte dividor = powers_of_two_byte[target - depth - 1];
    if (dividor == 0) {
        return node; // no need to dive then, we just set voxel anyway
    }

    byte3 positionn = (byte3) {
        byte_div(position.x, dividor),
        byte_div(position.y, dividor),
        byte_div(position.z, dividor)
    };
    byte3_modulus_byte(&position, dividor);

    byte i = byte3_octree_array_index(positionn);
    if (i >= 8) {
        zox_log_error("[fill_voctree] node index out of bounds: %i", i);
        return node;
    }

    VoxelNode* kids = get_children_VoxelNode(node);
    node = &kids[i];
    depth++;

    return fill_voctree(node, target, position, value, depth);
}


// t for terrain
VoxelNode* paint_voctree(VoxelNode* node, byte target, byte3 position, byte value, byte depth) {

    byte depth_reached = depth == target;

    if (!depth_reached && is_closed_VoxelNode(node)) {
        open_VoxelNode(node);
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            kids[i].value = 0;
        }
    }
    // wait this overrides child nodes, rather than reevaluating them
    if (depth_reached && node->value) {
        node->value = value;
    }
    if (depth_reached || !has_children_VoxelNode(node)) {
        return node;
    }

    byte dividor = powers_of_two_byte[target - depth - 1];
    if (dividor == 0) {
        return node; // no need to dive then, we just set voxel anyway
    }

    byte3 positionn = (byte3) {
        byte_div(position.x, dividor),
        byte_div(position.y, dividor),
        byte_div(position.z, dividor)
    };

    byte3_modulus_byte(&position, dividor);

    byte i = byte3_octree_array_index(positionn);

    if (i >= 8) {
        zox_log_error("[paint_voctree] node index out of bounds: %i", i);
        return node;
    }

    VoxelNode* kids = get_children_VoxelNode(node);
    node = &kids[i];
    depth++;

    return paint_voctree(node, target, position, value, depth);
}