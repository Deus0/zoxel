static inline byte float2_equals(float2 a, float2 b) {
    return a.x == b.x && a.y == b.y;
}

static inline void float2_make_zero(float2* value) {
    value->x = 0;
    value->y = 0;
}

static inline float2 float2_multiply(float2 a, float2 b) {
    return (float2) { a.x * b.x, a.y * b.y };
}

static inline float2 float2_multiply1(float2 a, float b) {
    return (float2) { a.x * b, a.y * b };
}

static inline void float2_multiply_float_p(float2 *input, const float mul) {
    input->x *= mul;
    input->y *= mul;
}

static inline void float2_multiply_float2_p(float2 *input, const float2 mul) {
    input->x *= mul.x;
    input->y *= mul.y;
}

static inline void float2_divide_float2(float2 *v, const float2 d) {
    if (d.x != 0) v->x /= d.x;
    if (d.y != 0) v->y /= d.y;
}

static inline void float2_divide_p(float2 *v, const float d) {
    if (d != 0) {
        v->x /= d;
        v->y /= d;
    }
}

static inline void float2_add_float2_p(float2 *input, const float2 add) {
    input->x += add.x;
    input->y += add.y;
}

static inline float2 float2_mul1(float2 input, float mul) {
    return (float2) { input.x * mul, input.y * mul };
}

static inline float2 float2_multiply_float(const float2 input, const float mul) {
    return (float2) { input.x * mul, input.y * mul };
}

static inline float2 float2_divide1(float2 input, float div) {
    if (!div) {
        return input;
    } else {
        return (float2) { input.x / div, input.y / div };
    }
}

static inline float2 float2_divide(float2 input, float2 div) {
    if (!div.x) {
        div.x = 1;
    }
    if (!div.y) {
        div.y = 1;
    }
    return (float2) { input.x / div.x, input.y / div.y };
}

static inline float2 float2_sub(const float2 a, const float2 b) {
    return (float2) { a.x - b.x, a.y - b.y };
}

static inline float2 float2_add(const float2 a, const float2 b) {
    return (float2) { a.x + b.x, a.y + b.y };
}

static inline float2 float2_subtract(const float2 a, const float2 b) {
    return (float2) { a.x - b.x, a.y - b.y };
}

static inline float float2_highest(const float2 input) {
    if (input.x > input.y) return input.x;
    else return input.y;
}

static inline float float2_lowest(const float2 input) {
    if (input.x < input.y) return input.x;
    else return input.y;
}

static inline void float2_normalize_p(float2 *input) {
    float length = sqrt(input->x * input->x + input->y * input->y);
    float2_divide_p(input, length);
}

static inline float2 float2_normalize(float2 input) {
    float length = sqrt(input.x * input.x + input.y * input.y);
    return float2_divide1(input, length);
}

static inline float2 normalize2D(float2 input) {
    float length = sqrt(input.x * input.x + input.y * input.y);
    if (!length) {
        return input;
    }
    return (float2) { input.x / length, input.y / length };
}


static inline float float2_distance(float2 a, float2 b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static inline float2 float2_single(float v) {
    return (float2) { v, v };
}

static inline float2 float2_mulf(float2 a, float m) {
    return (float2) { a.x * m, a.y * m };
}

static inline float2 float2_rotate(float2 v, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    return (float2) {
        v.x * c - v.y * s,
        v.x * s + v.y * c
    };
}
