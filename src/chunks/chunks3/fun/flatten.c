// Flattens a voxel octree into one array of uints, the layout gpu meshing reads (doc/tec/gpu_meshing.md)
//  - a node is one uint: its value in bits 0-7, the index of its first child in bits 8-31 (0: no children)
//  - node 0 is the root, a node's 8 children sit in a row, in octree order (x | y << 1 | z << 2, as
//      byte3_octree_array_index), breadth first so a level's nodes are together
//  - a linked node (a block's vox) is a leaf with its value, its link isn't followed
//  - a node without children is its value for its whole cube, as getv_VoxelNode reads it
//  - tst octree_flatten checks every voxel reads back the same as getv_VoxelNode

#define zox_flat_node_children_max 0xFFFFFF   // 24 bits of child index

static inline uint flat_node_value(uint node) {
    return node & 255u;
}

static inline uint flat_node_children(uint node) {
    return node >> 8;
}

static uint count_nodes_VoxelNode(const VoxelNode* node) {
    uint count = 1;
    if (has_children_VoxelNode(node)) {
        const VoxelNode* kids = (const VoxelNode*) node->ptr;
        for (byte i = 0; i < octree_length; i++) {
            count += count_nodes_VoxelNode(&kids[i]);
        }
    }
    return count;
}

// returns the node count, the nodes malloced into *out (free them), 0 on failure
uint flatten_VoxelNode(const VoxelNode* root, uint** out) {
    *out = NULL;
    if (!root) {
        return 0;
    }
    const uint count = count_nodes_VoxelNode(root);
    if (count > zox_flat_node_children_max) {
        zox_loge("[flatten_VoxelNode] [%u] nodes, more than a flat node can index", count);
        return 0;
    }
    uint* nodes = malloc(sizeof(uint) * count);
    const VoxelNode** order = malloc(sizeof(VoxelNode*) * count);
    if (!nodes || !order) {
        free(nodes);
        free(order);
        return 0;
    }
    // breadth first: order[i] is flat node i, its children get the next 8 free slots
    order[0] = root;
    uint next = 1;
    for (uint i = 0; i < count; i++) {
        const VoxelNode* node = order[i];
        uint children = 0;
        if (has_children_VoxelNode(node)) {
            children = next;
            const VoxelNode* kids = (const VoxelNode*) node->ptr;
            for (byte k = 0; k < octree_length; k++) {
                order[next++] = &kids[k];
            }
        }
        nodes[i] = (uint) node->value | (children << 8);
    }
    free(order);
    *out = nodes;
    return count;
}

// the voxel at position, at depth (its cube's size), read from flat nodes like walk_octree reads the octree
//  - the gpu mesher's voxel_at does the same (fun/gpu_mesh.c)
static inline byte flat_voxel_get(const uint* nodes, byte depth, int3 position) {
    const int length = octree_size(depth);
    if (position.x < 0 || position.y < 0 || position.z < 0 ||
        position.x >= length || position.y >= length || position.z >= length) {
        return 0;
    }
    uint node = 0;
    for (byte d = 0; d < depth; d++) {
        const uint children = flat_node_children(nodes[node]);
        if (!children) {
            break;
        }
        const byte shift = depth - d - 1;
        const uint index = ((position.x >> shift) & 1) | (((position.y >> shift) & 1) << 1) | (((position.z >> shift) & 1) << 2);
        node = children + index;
    }
    return (byte) flat_node_value(nodes[node]);
}
