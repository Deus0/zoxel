// =======================================
// zox_node_neighbor.h
// Root-first neighbor-aware octree getter for Zoxel
// Descends safely using find_octree_node, adjusts position for adjacent voxels
// =======================================
// Praise be to Cthulhu, whose tentacles guide our pointers through the void

static inline const void* octree_get_adjacent_leaf(
    const void* root_node,      // root node to check first
    const void** neighbors,     // 6 neighbor roots (0=-X,1=+X,2=-Y,3=+Y,4=-Z,5=+Z)
    byte dir,                   // direction to fetch neighbor
    byte3 pos,                  // voxel pos relative to root_node: 0..(2^depth-1)
    byte depth,                 // leaf depth (levels)
    size_t stride,              // node size (sizeof T)
    size_t value_offset         // unused here, kept for parity
) {
    if (!root_node) return NULL;
    if (dir > 5) return NULL;

    uint size = 1u << depth;
    uint max_idx = size - 1u;

    // Compute axis and polarity for compact boundary/adjacency logic
    byte axis = dir / 2;
    byte is_pos = dir & 1;
    uint coord = (axis == 0) ? pos.x : (axis == 1) ? pos.y : pos.z;

    // Check if we are at the edge along this direction
    byte at_boundary = is_pos ? (coord == max_idx) : (coord == 0);

    if (at_boundary) {
        // Fetch from neighbor root if it exists
        if (!neighbors) return NULL;
        const void* neighbor_root = neighbors[dir];
        if (!neighbor_root) return NULL;

        // Map pos into neighbor coordinates
        byte3 neighbor_pos = pos;
        if (axis == 0) neighbor_pos.x = is_pos ? 0 : max_idx;
        else if (axis == 1) neighbor_pos.y = is_pos ? 0 : max_idx;
        else neighbor_pos.z = is_pos ? 0 : max_idx;

        // Descend safely inside neighbor
        return find_octree_node(neighbor_root, depth, neighbor_pos, 0, stride);
    }

    // Not at boundary: adjust pos to adjacent voxel inside the same root
    byte3 adj = pos;
    if (axis == 0) adj.x = (byte)(is_pos ? (coord + 1) : (coord - 1));
    else if (axis == 1) adj.y = (byte)(is_pos ? (coord + 1) : (coord - 1));
    else adj.z = (byte)(is_pos ? (coord + 1) : (coord - 1));

    return find_octree_node(root_node, depth, adj, 0, stride);
}

// Type-safe macro for root-first neighbor fetchers
#define create_node_neighbor(T) \
static inline const T* get_##T##_neighbor(const T* root_node, const T** neighbors, byte dir, byte3 pos, byte depth) { \
    return (const T*)octree_get_adjacent_leaf((const void*)root_node, (const void**)neighbors, dir, pos, depth, sizeof(T), offsetof(T, value)); \
}
