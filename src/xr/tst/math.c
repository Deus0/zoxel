// --------------------------------------------------
// Math
//
//  -Copies of Zoxel functions, kept for test untiil derived from math/_.c
//
// --------------------------------------------------

static const float4x4 float4x4_identity = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

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

static inline float4x4 inverse_pr_to_matrix(
    float3 position,
    float4 rotation
) {
    const float x = rotation.x;
    const float y = rotation.y;
    const float z = rotation.z;
    const float w = rotation.w;
    const float r00 = 1 - 2 * (y * y + z * z);
    const float r01 = 2 * (x * y - z * w);
    const float r02 = 2 * (x * z + y * w);
    const float r10 = 2 * (x * y + z * w);
    const float r11 = 1 - 2 * (x * x + z * z);
    const float r12 = 2 * (y * z - x * w);
    const float r20 = 2 * (x * z - y * w);
    const float r21 = 2 * (y * z + x * w);
    const float r22 = 1 - 2 * (x * x + y * y);
    return (float4x4) {
        .x = { r00, r01, r02, 0 },
        .y = { r10, r11, r12, 0 },
        .z = { r20, r21, r22, 0 },
        .w = {
            -(r00 * position.x +
              r10 * position.y +
              r20 * position.z),
            -(r01 * position.x +
              r11 * position.y +
              r21 * position.z),
            -(r02 * position.x +
              r12 * position.y +
              r22 * position.z),
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


// Assuming Column Major matrix
static inline float3 matrix_to_scale(float4x4 m) {
    return (float3) {
        sqrtf(m.x.x * m.x.x + m.x.y * m.x.y + m.x.z * m.x.z),
        sqrtf(m.y.x * m.y.x + m.y.y * m.y.y + m.y.z * m.y.z),
        sqrtf(m.z.x * m.z.x + m.z.y * m.z.y + m.z.z * m.z.z)
    };
}

static inline float4 float4_divide_float(float4 input, float div) {
    return (float4) { input.x / div, input.y / div, input.z / div, input.w / div };
}

static inline float4 float4_normalize(float4 q) {
    float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return float4_divide_float(q, length);
}

// Assuming Column Major matrix (T * R * S)
static inline float4 matrix_to_rotation(float4x4 m) {
    float3 scale = matrix_to_scale(m);
    // Remove scale
    float m00 = m.x.x / scale.x;
    float m01 = m.y.x / scale.y;
    float m02 = m.z.x / scale.z;
    float m10 = m.x.y / scale.x;
    float m11 = m.y.y / scale.y;
    float m12 = m.z.y / scale.z;
    float m20 = m.x.z / scale.x;
    float m21 = m.y.z / scale.y;
    float m22 = m.z.z / scale.z;
    float4 q;
    float trace = m00 + m11 + m22;
    if (trace > 0.0f) {
        float s = sqrtf(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (m21 - m12) / s;
        q.y = (m02 - m20) / s;
        q.z = (m10 - m01) / s;
    } else if (m00 > m11 && m00 > m22) {
        float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    } else if (m11 > m22) {
        float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    } else {
        float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }
    return float4_normalize(q);
}


static inline float4 quaternion_inverse(float4 q) {
    return float4_divide_float(q, sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w));
}


float4x4 float4x4_inverse(const float4x4 matrix) {
    float4x4 inv = float4x4_identity;
    float* inv_ptr = (float*)&inv;
    float* matrix_ptr = (float*)&matrix;
    for (int i = 0; i < 4; i++) {
        int pivot_row = i;
        float max_val = fabsf(matrix_ptr[i * 4 + i]);
        for (int j = i + 1; j < 4; j++) {
            float val = fabsf(matrix_ptr[j * 4 + i]);
            if (val > max_val) {
                max_val = val;
                pivot_row = j;
            }
        }
        if (pivot_row != i) {
            // Swap rows in both the matrix and the result matrix
            for (int k = 0; k < 4; k++) {
                float temp = matrix_ptr[i * 4 + k];
                matrix_ptr[i * 4 + k] = matrix_ptr[pivot_row * 4 + k];
                matrix_ptr[pivot_row * 4 + k] = temp;

                temp = inv_ptr[i * 4 + k];
                inv_ptr[i * 4 + k] = inv_ptr[pivot_row * 4 + k];
                inv_ptr[pivot_row * 4 + k] = temp;
            }
        }
        // Divide the pivot row by the pivot element
        float pivot_element = matrix_ptr[i * 4 + i];
        for (int k = 0; k < 4; k++) {
            matrix_ptr[i * 4 + k] /= pivot_element;
            inv_ptr[i * 4 + k] /= pivot_element;
        }
        // Subtract multiples of the pivot row from other rows
        for (int j = 0; j < 4; j++) {
            if (j != i) {
                float factor = matrix_ptr[j * 4 + i];
                for (int k = 0; k < 4; k++) {
                    matrix_ptr[j * 4 + k] -= factor * matrix_ptr[i * 4 + k];
                    inv_ptr[j * 4 + k] -= factor * inv_ptr[i * 4 + k];
                }
            }
        }
    }
    return inv;
}

// a copy of cameras/fun/projection_angles.c camera_projection_from_angles
static inline float4x4 xr_create_projection_matrix(
    float4 fov,
    float n,
    float z
) {
    const float l = tanf(fov.x);
    const float r = tanf(fov.y);
    const float d = tanf(fov.z);
    const float u = tanf(fov.w);
    return (float4x4){
        .x = {
            2.0f / (r - l),
            0,
            0,
            0
        },
        .y = {
            0,
            2.0f / (d - u),
            0,
            0
        },
        .z = {
            (r + l) / (r - l),
            (d + u) / (d - u),
            -(z + n) / (z - n),
            -1.0f
        },
        .w = {
            0,
            0,
            -(2.0f * z * n) / (z - n),
            0
        }
    };
}

// --------------------------------------------------
// Copies for this standalone app only, until it's retired for an engine scene: the engine's own (_/maths),
//  which xr/fun now uses, and xr/fun/math.c's old pose helpers this app calls
// --------------------------------------------------

static const float4x4 float4x4_zero = { 0 };

static inline float4x4 float4x4_multiply(float4x4 a, float4x4 b) {
    float4x4 c = float4x4_zero;
    float* a2 = (float*) &a;
    float* b2 = (float*) &b;
    float* c2 = (float*) &c;
    for (unsigned j = 0; j < 16; j++) {
        unsigned i = j % 4;
        unsigned j4 = j & 12;
        c2[j] = a2[j4 + 0] * b2[i + 0] + a2[j4 + 1] * b2[i + 4]
            + a2[j4 + 2] * b2[i + 8] + a2[j4 + 3] * b2[i + 12];
    }
    return c;
}

static inline float3 float4x4_get_position(float4x4 matrix) {
    return (float3) { matrix.w.x, matrix.w.y, matrix.w.z };
}

static inline float4x4 float4x4_position(const float3 position) {
    float4x4 matrix = float4x4_identity;
    matrix.w.x = position.x;
    matrix.w.y = position.y;
    matrix.w.z = position.z;
    return matrix;
}

static inline float4x4 float4x4_rotation(const float4 rotation) {
    float4x4 m = float4x4_identity;
    float xx = 2.0 * rotation.x * rotation.x;
    float xy = 2.0 * rotation.x * rotation.y;
    float xz = 2.0 * rotation.x * rotation.z;
    float yy = 2.0 * rotation.y * rotation.y;
    float yz = 2.0 * rotation.y * rotation.z;
    float zz = 2.0 * rotation.z * rotation.z;
    float wx = 2.0 * rotation.w * rotation.x;
    float wy = 2.0 * rotation.w * rotation.y;
    float wz = 2.0 * rotation.w * rotation.z;
    m.x.x = (1.0 - yy - zz);
    m.x.y = (xy + wz);
    m.x.z = (xz - wy);
    m.y.x = (xy - wz);
    m.y.y = (1.0 - xx - zz);
    m.y.z = (yz + wx);
    m.z.x = (xz + wy);
    m.z.y = (yz - wx);
    m.z.z = (1.0 - xx - yy);
    return m;
}

static inline float4x4 float4x4_transform(float3 position,float4 rotation) {
    float4x4 position_matrix = float4x4_position(position);
    float4x4 rotation_matrix = float4x4_rotation(float4_normalize(rotation));
    return float4x4_multiply(rotation_matrix, position_matrix);
}

static inline float4x4 pr_to_matrix(
    float3 position,
    float4 rotation
) {
    const float x = rotation.x;
    const float y = rotation.y;
    const float z = rotation.z;
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

    return (float4x4) {
        .x = {
            1 - 2 * (yy + zz),
            2 * (xy + wz),
            2 * (xz - wy),
            0
        },
        .y = {
            2 * (xy - wz),
            1 - 2 * (xx + zz),
            2 * (yz + wx),
            0
        },
        .z = {
            2 * (xz + wy),
            2 * (yz - wx),
            1 - 2 * (xx + yy),
            0
        },
        .w = {
            position.x,
            position.y,
            position.z,
            1
        }
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