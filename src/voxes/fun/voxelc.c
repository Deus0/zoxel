// c for colos
/*VoxelNode* set_voxelc(
    VoxelNode* node,
    byte tdepth,
    byte3 position,
    byte value,
    byte depth
) {
    byte depth_reached = depth == tdepth;

    if (!depth_reached && is_closed_VoxelNode(node)) {
        open_VoxelNode(node);
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            // set to parent value
            kids[i].value = node->value;
        }
    }

    // wait this overrides child nodes, rather than reevaluating them
    if (depth_reached) {
        node->value = value;
    //}
    //if (depth_reached || !has_children_VoxelNode(node)) {
        return node;
    }

    byte dividor = powers_of_two_byte[tdepth - depth - 1];
    if (!dividor) {
        return node;
    }

    byte3 positionn = (byte3) {
        position.x / dividor,
        position.y / dividor,
        position.z / dividor
    };

    byte i = byte3_octree_array_index(positionn);
    if (i >= 8) {
        zox_log_error("[set_voxelc] node index out of bounds: %i", i);
        return node;
    }

    VoxelNode* kids = get_children_VoxelNode(node);
    node = &kids[i];
    depth++;
    byte3_modulus_byte(&position, dividor);

    return set_voxelc(
        node,
        tdepth,
        position,
        value,
        depth);
}*/