static inline byte float4_equals(float4 a, float4 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

static inline void print_float4(float4 input) {
    zox_log("    Float4 [%f %f %f %f]\n", input.x, input.y, input.z, input.w);
}

static inline float4 float4_from_float3(float3 v, float w) {
    return (float4) { v.x, v.y, v.z, w };
}

static inline float4 float4_multiply_float(float4 input, float mul) {
    return (float4) { input.x * mul, input.y * mul, input.z * mul, input.w * mul };
}

static inline float4 float4_divide_float(float4 input, float div) {
    return (float4) { input.x / div, input.y / div, input.z / div, input.w / div };
}

static inline float4 float4_subtract(float4 a, float4 b) {
    return (float4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

static inline float4 float4_add(float4 a, float4 b) {
    return (float4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

static inline float4 float4_divide_float_safe(float4 input, float div) {
    if (div == 0) return input;
    return float4_divide_float(input, div);
}

static inline float float4_dot(float4 a, float4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float float4_length(float4 v) {
    return sqrt(float4_dot(v, v));
}

static inline float4 float4_normalize(float4 q) {
    float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return float4_divide_float(q, length);
}

static inline void quaternion_rotate_quaternion_p(float4 *output, float4 q2) {
    float4 q1 = *output;
    output->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    output->x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    output->y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    output->z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
}

static inline float4 quaternion_rotate(float4 q1, float4 q2) {
    float4 output;
    output.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    output.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    output.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    output.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return output;
}

//! Also called Conjugation in maths. Apparently x needs to be position still?
// confirmed here https://www.youtube.com/watch?v=A6A0rpV9ElA
static inline float4 float4_inverse(float4 input) {
    // return (float4) { input.x, -input.y, -input.z, -input.w };
    return (float4) { -input.x, -input.y, -input.z, input.w };
}

static inline float4 quaternion_inverse(float4 q) {
    return float4_divide_float(q, sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w));
}

static inline float4 float3_to_float4(float3 input) {
    return (float4) { input.x, input.y, input.z, 0 };
}

static inline float3 float4_xyz(float4 input) {
    return (float3) { input.x, input.y, input.z };
}

static inline void float4_multiply_float_p(float4* input, float mul) {
    input->x *= mul;
    input->y *= mul;
    input->z *= mul;
    input->w *= mul;
}

// This output is wrong - [0, 0.7, 0, 0.7] (rotate left)
/**
 * t = 2 * cross(q.xyz, v)
 * v' = v + q.w * t + cross(q.xyz, t)
 */
static inline float3 float4_rotate_float3(float4 rotation, float3 value) {
    float3 rotationXYZ = float4_xyz(rotation);
    float3 t = float3_scale(float3_cross(rotationXYZ, value), 2.0f);
    float3 crossB = float3_cross(rotationXYZ, t);
    float3 scaledT = float3_scale(t, rotation.w);
    return float3_add(value, float3_add(scaledT, crossB));
}

static inline void float4_rotate_float3_p(float4 rotation, float3 *value) {
    float3 rotationXYZ = float4_xyz(rotation);
    float3 t = float3_scale(float3_cross(rotationXYZ, (float3) { value->x, value->y, value->z }), 2.0f);
    float3 crossB = float3_cross(rotationXYZ, t);
    float3 scaledT = float3_scale(t, rotation.w);
    float3_add_float3_p(value, float3_add(scaledT, crossB));
}

static inline float3 move_along_direction(float3 position, float4 rotation, float length) {
    return float3_add(position, float4_rotate_float3(rotation, (float3) { 0, 0, length }));
}

static inline float3 float4_rotate_bounds(float4 rotation, float3 extents) {
    float3 xAxis = (float3){1.0f, 0.0f, 0.0f};
    float3 yAxis = (float3){0.0f, 1.0f, 0.0f};
    float3 zAxis = (float3){0.0f, 0.0f, 1.0f};
    float3 rx = float4_rotate_float3(rotation, xAxis);
    float3 ry = float4_rotate_float3(rotation, yAxis);
    float3 rz = float4_rotate_float3(rotation, zAxis);
    // Project extents onto rotated axes
    float3 abs_rx = float3_abs(rx);
    float3 abs_ry = float3_abs(ry);
    float3 abs_rz = float3_abs(rz);
    return float3_add(
        float3_add(
            float3_scale(abs_rx, extents.x),
            float3_scale(abs_ry, extents.y)
        ),
        float3_scale(abs_rz, extents.z)
    );
}
