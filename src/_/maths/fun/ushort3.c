const ushort3 ushort3_zero = (ushort3) { 0, 0, 0 };

static inline byte3 ushort3_to_byte3(ushort3 a) {
    return (byte3) {
        a.x > 255 ? 255 : a.x,
        a.y > 255 ? 255 : a.y,
        a.z > 255 ? 255 : a.z
    };
}