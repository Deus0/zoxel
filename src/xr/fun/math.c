static inline void inverse_matrix_to_pr(
    float4x4 matrix,
    float3* position,
    float4* rotation
) {
    // Inverse PR matrix:
    //
    // [ R^T  -R^T p ]
    // [ 0       1   ]
    //
    // Transpose the rotation to recover the original rotation.

    const float r00 = matrix.x.x;
    const float r01 = matrix.y.x;
    const float r02 = matrix.z.x;
    const float r10 = matrix.x.y;
    const float r11 = matrix.y.y;
    const float r12 = matrix.z.y;
    const float r20 = matrix.x.z;
    const float r21 = matrix.y.z;
    const float r22 = matrix.z.z;

    position->x = -(r00 * matrix.w.x +
                    r10 * matrix.w.y +
                    r20 * matrix.w.z);

    position->y = -(r01 * matrix.w.x +
                    r11 * matrix.w.y +
                    r21 * matrix.w.z);

    position->z = -(r02 * matrix.w.x +
                    r12 * matrix.w.y +
                    r22 * matrix.w.z);

    const float trace = r00 + r11 + r22;

    if (trace > 0.0f) {
        const float s = sqrtf(trace + 1.0f) * 2.0f;
        rotation->w = 0.25f * s;
        rotation->x = (r21 - r12) / s;
        rotation->y = (r02 - r20) / s;
        rotation->z = (r10 - r01) / s;
    } else if (r00 > r11 && r00 > r22) {
        const float s = sqrtf(1.0f + r00 - r11 - r22) * 2.0f;
        rotation->w = (r21 - r12) / s;
        rotation->x = 0.25f * s;
        rotation->y = (r01 + r10) / s;
        rotation->z = (r02 + r20) / s;
    } else if (r11 > r22) {
        const float s = sqrtf(1.0f + r11 - r00 - r22) * 2.0f;
        rotation->w = (r02 - r20) / s;
        rotation->x = (r01 + r10) / s;
        rotation->y = 0.25f * s;
        rotation->z = (r12 + r21) / s;
    } else {
        const float s = sqrtf(1.0f + r22 - r00 - r11) * 2.0f;
        rotation->w = (r10 - r01) / s;
        rotation->x = (r02 + r20) / s;
        rotation->y = (r12 + r21) / s;
        rotation->z = 0.25f * s;
    }
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