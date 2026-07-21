static inline byte byte_div(byte v, byte d) {
    if (!d) {
        return v;
    }
    return v / d;
}

static inline byte byte_floorf(float v) {
    return (byte) ((int) floorf(v));
}

static inline byte byte_divf(byte v, float d) {
    if (!d) {
        return v;
    }
    return byte_floorf(v / d);
}

static inline byte byte_min(byte a, byte b) {
    return a < b ? a : b;
}

static inline byte byte_max(byte a, byte b) {
    return a > b ? a : b;
}

static inline byte byte_clamp(int a, byte low, byte high) {
    if (a < low) return low;
    else if (a > high) return high;
    else return a;
}
