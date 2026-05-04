static inline byte byte_div(byte v, byte d) {
    if (!d) {
        return v;
    }
    return v / d;
}