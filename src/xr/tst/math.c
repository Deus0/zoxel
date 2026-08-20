// --------------------------------------------------
// Math
//
//  -Copies of Zoxel functions, kept for test untiil derived from math/_.c
//
// --------------------------------------------------

static float random_float(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

static inline float3 float3_add(float3 a, float3 b) {
    return (float3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline float3 float3_sub(float3 a, float3 b) {
    return (float3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

static float3 rotate_y(float3 v, float angle) {
    const float c = cosf(angle);
    const float s = sinf(angle);
    return (float3){
        v.x * c + v.z * s,
        v.y,
        -v.x * s + v.z * c
    };
}

static float4 quaternion_multiply(float4 a, float4 b) {
    return (float4){
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
}

static inline float4 euler_to_quaternion(float3 e) {
    const float cx = cosf(e.x * 0.5f);
    const float sx = sinf(e.x * 0.5f);
    const float cy = cosf(e.y * 0.5f);
    const float sy = sinf(e.y * 0.5f);
    const float cz = cosf(e.z * 0.5f);
    const float sz = sinf(e.z * 0.5f);
    return (float4){
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    };
}

static float4x4 matrix_multiply(float4x4 a, float4x4 b) {
    float4x4 result;
    for (int column = 0; column < 4; column++) {
        float4 bc =
            column == 0 ? b.x :
            column == 1 ? b.y :
            column == 2 ? b.z :
            b.w;
        float4 *rc =
            column == 0 ? &result.x :
            column == 1 ? &result.y :
            column == 2 ? &result.z :
            &result.w;
        rc->x =
            a.x.x * bc.x +
            a.y.x * bc.y +
            a.z.x * bc.z +
            a.w.x * bc.w;
        rc->y =
            a.x.y * bc.x +
            a.y.y * bc.y +
            a.z.y * bc.z +
            a.w.y * bc.w;
        rc->z =
            a.x.z * bc.x +
            a.y.z * bc.y +
            a.z.z * bc.z +
            a.w.z * bc.w;
        rc->w =
            a.x.w * bc.x +
            a.y.w * bc.y +
            a.z.w * bc.z +
            a.w.w * bc.w;
    }
    return result;
}

static float4x4 prs_to_transform_matrix(
    float3 p,
    float4 q,
    float3 s
) {
    const float x = q.x;
    const float y = q.y;
    const float z = q.z;
    const float w = q.w;
    return (float4x4){
        .x = {
            (1 - 2 * (y * y + z * z)) * s.x,
            (2 * (x * y + z * w)) * s.x,
            (2 * (x * z - y * w)) * s.x,
            0
        },
        .y = {
            (2 * (x * y - z * w)) * s.y,
            (1 - 2 * (x * x + z * z)) * s.y,
            (2 * (y * z + x * w)) * s.y,
            0
        },
        .z = {
            (2 * (x * z + y * w)) * s.z,
            (2 * (y * z - x * w)) * s.z,
            (1 - 2 * (x * x + y * y)) * s.z,
            0
        },
        .w = {
            p.x,
            p.y,
            p.z,
            1
        }
    };
}

static float4x4 create_view_matrix(
    float3 position,
    float4 rotation
) {
    const float x = -rotation.x;
    const float y = -rotation.y;
    const float z = -rotation.z;
    const float w = rotation.w;
    const float xx = x * x;
    const float yy = y * y;
    const float zz = z * z;
    const float xy = x * y;
    const float xz = x * z;
    const float yz = y * z;
    const float wx = w * x;
    const float wy = w * y;
    const float wz = w * z;
    const float r00 = 1 - 2 * (yy + zz);
    const float r01 = 2 * (xy - wz);
    const float r02 = 2 * (xz + wy);
    const float r10 = 2 * (xy + wz);
    const float r11 = 1 - 2 * (xx + zz);
    const float r12 = 2 * (yz - wx);
    const float r20 = 2 * (xz - wy);
    const float r21 = 2 * (yz + wx);
    const float r22 = 1 - 2 * (xx + yy);
    return (float4x4){
        .x = { r00, r10, r20, 0 },
        .y = { r01, r11, r21, 0 },
        .z = { r02, r12, r22, 0 },
        .w = {
            -(r00 * position.x + r01 * position.y + r02 * position.z),
            -(r10 * position.x + r11 * position.y + r12 * position.z),
            -(r20 * position.x + r21 * position.y + r22 * position.z),
            1
        }
    };
}

static float4 quaternion_conjugate(float4 q) {
    return (float4) {
        -q.x,
        -q.y,
        -q.z,
        q.w
    };
}

static float3 quaternion_rotate_vector(
    float4 q,
    float3 v
) {
    float4 vq = { v.x, v.y, v.z, 0 };
    float4 result = quaternion_multiply(
        quaternion_multiply(q, vq),
        quaternion_conjugate(q)
    );
    return (float3) {
        result.x,
        result.y,
        result.z
    };
}