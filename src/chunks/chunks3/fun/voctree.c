// used by physics and raycasting
// i think const was the issue
byte get_sub_node_voxel(const VoxelNode* node, byte3* positionl, byte depth) {
    if (!node) {
        return 0;
    }
    if (depth == 0 || !has_children_VoxelNode(node)) {
        return node->value;
    }
    depth--;
    byte dividor = powers_of_two_byte[depth];
    byte3 positionn = (byte3) {
        positionl->x / dividor,
        positionl->y / dividor,
        positionl->z / dividor
    };
    byte i = byte3_octree_array_index(positionn);
    if (i >= 8) {
        zox_logw("[get_sub_node_voxel] Node Index OOB: %i - depth [%i] - positionl [%ix%ix%i] - positionn [%ix%ix%i] dividor [%i]", i, depth, positionl->x, positionl->y, positionl->z, positionn.x, positionn.y, positionn.z, dividor);
        return node->value;
    }
    byte3_modulus_byte(positionl, dividor); // leftover goes here
    VoxelNode* kids = get_children_VoxelNode(node);
    if (!kids) {
        zox_log_error("node kids null!");
        return node->value;
    }
    return get_sub_node_voxel(&kids[i], positionl, depth);
}

void fill_octree(VoxelNode* voctree, byte voxel, byte depth) {
    if (!voctree) {
        return;
    }
    voctree->value = voxel;
    if (depth == 0) {
        return;
    }
    depth--;
    open_VoxelNode(voctree);
    VoxelNode* kids = get_children_VoxelNode(voctree);
    for (byte i = 0; i < octree_length; i++) {
        fill_octree(&kids[i], voxel, depth);
    }
}
