static inline byte2 byte2_multiply_int(const byte2 input, const int mul) {
    return (byte2) { input.x * mul, input.y * mul };
}

static inline int byte2_array_index(const byte2 input, const byte2 size) {
    return input.x + input.y * size.x;
}

static inline byte byte2_on_edge(const byte2 pos, const byte2 size) {
    return pos.x == 0 || pos.y == 0 || pos.x == size.x - 1 || pos.y == size.y - 1;
}

static inline byte2 byte2_single(const byte a) {
    return (byte2) { a, a };
}

static inline byte2 byte2_add(byte2 a, byte2 b) {
#ifdef zox_safety_checks
    if (a.x + b.x > 255 || a.y + b.y > 255) {
        zox_loge("byte2_add adding more");
    }
#endif
    return (byte2) {
        byte_clamp(a.x + b.x, 0, 255),
        byte_clamp(a.y + b.y, 0, 255)
    };
}
