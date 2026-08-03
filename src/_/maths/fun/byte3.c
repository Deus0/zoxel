static inline byte byte3_equals(byte3 a, byte3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline byte3 byte3_single(byte a) {
    return (byte3) { a, a, a };
}

static inline byte3 byte3_half(byte3 v) {
    return (byte3) { v.x / 2, v.y / 2, v.z / 2 };
}

static inline int byte3_array_index(byte3 input, byte3 size) {
    return input.z + size.z * (input.y + size.y * input.x);
}

static inline int byte3_array_indexl(byte3 input, byte length) {
    return input.z + length * (input.y + length * input.x);
}

static inline byte3 byte3_add(byte3 a, byte3 b) {
    return (byte3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline void byte3_add_byte3_p(byte3 *value, byte3 add) {
    value->x += add.x;
    value->y += add.y;
    value->z += add.z;
}

static inline byte3 byte3_div1(byte3 a, byte d) {
    if (d == 0) {
        zox_loge("byte3_div1 0 error");
        return a;
    }
    return (byte3) { a.x / d, a.y / d, a.z / d };
}

static inline void byte3_modulus_byte(byte3 *value, byte moduli) {
    value->x %= moduli;
    value->y %= moduli;
    value->z %= moduli;
}

static inline byte3 byte3_left(byte3 input) {
    return (byte3) { input.x - 1, input.y, input.z };
}

static inline byte3 byte3_right(byte3 input) {
    return (byte3) { input.x + 1, input.y, input.z };
}

static inline byte3 byte3_down(byte3 input) {
    return (byte3) { input.x, input.y - 1, input.z };
}

static inline byte3 byte3_up(byte3 input) {
    return (byte3) { input.x, input.y + 1, input.z };
}

static inline byte3 byte3_back(byte3 input) {
    return (byte3) { input.x, input.y, input.z - 1 };
}

static inline byte3 byte3_front(byte3 input) {
    return (byte3) { input.x, input.y, input.z + 1 };
}

static inline byte3 byte3_reverse_left(byte3 input, byte3 size) {
    return (byte3) { size.x - 1, input.y, input.z };
}

static inline byte3 byte3_reverse_right(byte3 input) {
    return (byte3) { 0, input.y, input.z };
}

static inline byte3 byte3_reverse_down(byte3 input, byte3 size) {
    return (byte3) { input.x, size.y - 1, input.z };
}

static inline byte3 byte3_reverse_up(byte3 input) {
    return (byte3) { input.x, 0, input.z };
}

static inline byte3 byte3_reverse_back(byte3 input, byte3 size) {
    return (byte3) { input.x, input.y, size.z - 1 };
}

static inline byte3 byte3_reverse_front(byte3 input) {
    return (byte3) { input.x, input.y, 0 };
}

static inline byte byte3_in_bounds(byte3 input, byte3 size) {
    return (input.x < size.x && input.y < size.y && input.z < size.z);
}

static inline byte byte3_on_edge(const byte3 pos, const byte3 size) {
    return pos.x == 0 || pos.y == 0 || pos.z == 0 || pos.x == size.x - 1 || pos.y == size.y - 1 || pos.z == size.z - 1;
}

static inline byte byte3_on_edge_xz(const byte3 pos, const byte3 size) {
    return pos.x == 0 || pos.z == 0 || pos.x == size.x - 1 || pos.z == size.z - 1;
}

static inline byte byte3_on_edges(const byte3 pos, const byte3 size) {
    byte edges_count = 0;
    if (pos.x == 0 || pos.x == size.x - 1) edges_count++;
    if (pos.y == 0 || pos.y == size.y - 1) edges_count++;
    if (pos.z == 0 || pos.z == size.z - 1) edges_count++;
    return edges_count >= 2;
}

static inline byte3 byte3_mul1(byte3 a, byte m) {
    return (byte3) { a.x * m, a.y * m, a.z * m };
}

static inline void byte3_multiply_byte(byte3* input, byte mul) {
    input->x *= mul;
    input->y *= mul;
    input->z *= mul;
}

static inline byte3 byte3_scalef(byte3 v, float m) {
    return (byte3) { (int) (v.x * m), (int) (v.y * m), (int) (v.z * m) };
}

static inline byte3 byte3_scale3f(byte3 v, float3 m) {
    return (byte3) { (int) (v.x * m.x), (int) (v.y * m.y), (int) (v.z * m.z) };
}

static inline byte3 byte3_scale(byte3 v, int m) {
    return (byte3) { v.x * m, v.y * m, v.z * m };
}

static inline byte3 byte3_inverse_scale(byte3 v, int div) {
    if (!div) {
        return v;
    }
    return (byte3) { v.x / div, v.y / div, v.z / div };
}


static inline byte byte3_distance(byte3 a, byte3 b) {
    return (byte) ((int) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z)));
}