static inline byte is_in_octree(const void* node, size_t stride, size_t value_offset, byte check) {
    if (!node) {
        return 0;
    }
    byte value = *(const byte*)((const char*) node + value_offset);
    if (value == check) {
        return 1;
    }
    const void* children = *(const void* const*) node;
    if (!children) {
        return 0;
    }
    for (byte i = 0; i < 8; i++) {
        const void* child = (const char*) children + (i * stride);
        byte is_in = is_in_octree(child, stride, value_offset, check);
        if (is_in) {
            return 1;
        }
    }
    return 0;
}
