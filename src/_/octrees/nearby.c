// NOTE: Handles one chunk distance nearby, using offset [-1,+1]
static inline const void* octree_get_nearby(const void** octrees, byte3 position, byte depth, sbyte3 offset, size_t stride) {
    if (!octrees) {
        return NULL;
    }
    if (depth >= 7) {
        zox_logw("Depth out of bounds [%i]", depth);
        return NULL;
    }
    short size = octree_size(depth);
    sbyte nx = position.x + offset.x;
    sbyte ny = position.y + offset.y;
    sbyte nz = position.z + offset.z;
    sbyte rx = 0;
    sbyte ry = 0;
    sbyte rz = 0;
    if (nx < 0) {
        rx = -1;
        nx += size;
    } else if (nx >= size) {
        rx = 1;
        nx -= size;
    }
    if (ny < 0) {
        ry = -1;
        ny += size;
    } else if (ny >= size) {
        ry = 1;
        ny -= size;
    }
    if (nz < 0) {
        rz = -1;
        nz += size;
    } else if (nz >= size) {
        rz = 1;
        nz -= size;
    }
    byte index = OCTREE_INDEX(rx, ry, rz);
    const void* node = octrees[index];
    if (!node) {
        return NULL;
    }
    byte3 pos = {
        (byte) nx,
        (byte) ny,
        (byte) nz
    };
    return get_octree(node, depth, pos, 0, stride);
}

// NOTE: 27 neighbors
#define create_octree_get_nearby(T) \
static inline const T* get_nearby_##T(const T** octrees, byte3 position, byte depth, sbyte3 offset) { \
    return (const T*) octree_get_nearby((const void**) octrees, position, depth, offset, sizeof(T)); \
} \
static inline byte getv_nearby_##T(const T** octrees, byte3 position, byte depth, sbyte3 offset) { \
    const T* thing = (const T*) octree_get_nearby((const void**) octrees, position, depth, offset, sizeof(T)); \
    return thing ? thing->value : 0; \
}

// TODO: Function to grab 27 neighbors from chunk + neighbors
// TODO: remove depth from get_octree and just use target depth
