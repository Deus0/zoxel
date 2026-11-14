// Flattens an octree into one array of uints, the layout gpu shaders read (doc/tec/gpu_meshing.md)
//  - a node is one uint: its value in bits 0-7, the index of its first child in bits 8-31 (0: no children)
//  - node 0 is the root, a node's 8 children sit in a row, in octree order (x | y << 1 | z << 2, as
//      byte3_octree_array_index), breadth first so a level's nodes are together
//  - linked nodes are leaves with their value (octree_children skips links)
//  - a node without children is its value for its whole cube, as walk_octree reads it
//  - max_values: an open node holds the max of its leaves, not its own value (light trees: a coarse read is the
//      brightest light under it, as getv_nearby_max_LightNode; an open light node's own value is stale)
//  - tst octree_flatten (voxels), gpu_mesh_lights (lights)

#define zox_flat_node_children_max 0xFFFFFF   // 24 bits of child index

static inline uint flat_node_value(uint node) {
    return node & 255u;
}

static inline uint flat_node_children(uint node) {
    return node >> 8;
}

static uint flatten_octree_count(const void* node, size_t stride, size_t type_offset) {
    uint count = 1;
    const void* kids = octree_children(node, type_offset);
    if (kids) {
        for (byte i = 0; i < octree_length; i++) {
            count += flatten_octree_count(octree_child((void*) kids, i, stride), stride, type_offset);
        }
    }
    return count;
}

// returns the node count, the nodes malloced into *out (free them), 0 on failure
uint flatten_octree(const void* root, size_t stride, size_t type_offset, byte max_values, uint** out) {
    *out = NULL;
    if (!root) {
        return 0;
    }
    const uint count = flatten_octree_count(root, stride, type_offset);
    if (count > zox_flat_node_children_max) {
        zox_loge("[flatten_octree] [%u] nodes, more than a flat node can index", count);
        return 0;
    }
    uint* nodes = malloc(sizeof(uint) * count);
    const void** order = malloc(sizeof(void*) * count);
    if (!nodes || !order) {
        free(nodes);
        free(order);
        return 0;
    }
    // breadth first: order[i] is flat node i, its children get the next 8 free slots
    order[0] = root;
    uint next = 1;
    for (uint i = 0; i < count; i++) {
        const void* node = order[i];
        const void* kids = octree_children(node, type_offset);
        uint children = 0;
        if (kids) {
            children = next;
            for (byte k = 0; k < octree_length; k++) {
                order[next++] = octree_child((void*) kids, k, stride);
            }
        }
        const byte value = *(const byte*) ((const char*) node + octree_value_offset);
        nodes[i] = (uint) value | (children << 8);
    }
    free(order);
    // children come after their parents, so going backwards a parent sees its children's maxes
    if (max_values) {
        for (uint i = count; i-- > 0;) {
            const uint children = flat_node_children(nodes[i]);
            if (!children) {
                continue;
            }
            uint most = 0;
            for (byte k = 0; k < octree_length; k++) {
                const uint value = flat_node_value(nodes[children + k]);
                most = value > most ? value : most;
            }
            nodes[i] = most | (children << 8);
        }
    }
    *out = nodes;
    return count;
}

// the value at position, at depth (its cube's size), read from flat nodes like walk_octree reads the octree,
//  outside: what's past the octree's edge
//  - the gpu mesher's voxel_at does the same (chunks3/fun/gpu_mesh.c)
static inline byte flat_octree_get(const uint* nodes, byte depth, int3 position, byte outside) {
    const int length = octree_size(depth);
    if (position.x < 0 || position.y < 0 || position.z < 0 ||
        position.x >= length || position.y >= length || position.z >= length) {
        return outside;
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
