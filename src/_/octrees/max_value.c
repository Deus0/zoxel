static inline byte get_octree_max_value(const void* node, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    byte max = *(const byte*)((const char*) node + value_offset);
    const void* children = *(const void* const*) node;
    if (!children) {
        return max;
    }
    for (byte i = 0; i < 8; i++) {
        const void* child = (const char*) children + (i * stride);
        byte new_max = get_octree_max_value(child, stride, value_offset);
        if (new_max > max) {
            max = new_max;
        }
    }
    return max;
}
