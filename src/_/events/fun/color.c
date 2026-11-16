static inline byte color_equal(const color a, const color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

static inline byte is_color_null(color a) {
    return color_equal(a, color_null);
}

static inline float4 color_to_float4(color input) {
    return (float4) {
        ((int) input.r) / 255.0f,
        ((int) input.g) / 255.0f,
        ((int) input.b) / 255.0f,
        ((int) input.a) / 255.0f
    };
}

static inline color color_mix(color a, color b, float m) {
    int r = (int)(a.r * m + b.r * (1 - m));
    int g = (int)(a.g * m + b.g * (1 - m));
    int b2 = (int)(a.b * m + b.b * (1 - m));
    int a2 = (int)(a.a * m + b.a * (1 - m));
    return (color) {
        (byte)(r > 255 ? 255 : (r < 0 ? 0 : r)),
        (byte)(g > 255 ? 255 : (g < 0 ? 0 : g)),
        (byte)(b2 > 255 ? 255 : (b2 < 0 ? 0 : b2)),
        (byte)(a2 > 255 ? 255 : (a2 < 0 ? 0 : a2)) };
}

static inline color color_multiply_float(color v, float multiplier) {
    int r = (int)(v.r * multiplier);
    int g = (int)(v.g * multiplier);
    int b = (int)(v.b * multiplier);
    v.r = (byte)(r > 255 ? 255 : (r < 0 ? 0 : r));
    v.g = (byte)(g > 255 ? 255 : (g < 0 ? 0 : g));
    v.b = (byte)(b > 255 ? 255 : (b < 0 ? 0 : b));
    return v;
}

static inline color color_grayscale(byte value) {
    return (color) { value, value, value, 255 };
}

static inline color color_grayscale_a(byte v, byte a) {
    return (color) { v, v, v, a };
}

static inline color color_mutate(color c, byte m) {
    int dr = (int) c.r + (-m + rand() % (2 * m + 1));
    int dg = (int) c.g + (-m + rand() % (2 * m + 1));
    int db = (int) c.b + (-m + rand() % (2 * m + 1));
    // Clamp to [0, 255]
    c.r = (byte) (dr < 0 ? 0 : (dr > 255 ? 255 : dr));
    c.g = (byte) (dg < 0 ? 0 : (dg > 255 ? 255 : dg));
    c.b = (byte) (db < 0 ? 0 : (db > 255 ? 255 : db));
    return c;
}