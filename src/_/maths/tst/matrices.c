// tst: maths_matrices, float4x4 (src/_/maths/fun/float4x4.c), pure logic, ready on the first frame
//  - a transform (position, rotation, scale) moves a point as the quaternion turns it, scales it, then moves it
//  - multiply(a, b) is a then b (the model, then the camera: render_chunks.c): (a b) v = b (a v); identity changes nothing
//  - inverse undoes: m^-1 m = identity, for transforms with and without scale; inverse_safe agrees
//  - transpose twice is the matrix; get_position reads back its position
//  - run: bash bsh/tst.sh maths_matrices
static inline float zox_tst_matrix_error(const float4x4 a, const float4x4 b) {
    const float* x = (const float*) &a;
    const float* y = (const float*) &b;
    float worst = 0;
    for (int i = 0; i < 16; i++) {
        const float d = fabsf(x[i] - y[i]);
        worst = d > worst || d != d ? d : worst;
    }
    return worst;
}

static inline float4x4 zox_tst_matrix_rand_transform(byte scaled) {
    const float scale = scaled ? 0.5f + (zox_tst_maths_rand() + 1) : 1;
    return float4x4_transform_scale(zox_tst_maths_rand3(50), zox_tst_maths_rand_rotation(), scale);
}

void zox_tst_maths_matrices_check(ecs* world) {
    (void) world;
    zox_tst_maths_seed = 4321;
    zox_tst_maths_worst moves = { 0 }, composes = { 0 }, identity = { 0 }, inverse = { 0 }, inverse_scaled = { 0 },
        inverse_safe = { 0 }, transpose = { 0 }, position = { 0 }, rotation_only = { 0 };
    const float4x4 one = float4x4_identity;
    for (int i = 0; i < zox_tst_maths_samples; i++) {
        const float3 p = zox_tst_maths_rand3(50);
        const float4 q = zox_tst_maths_rand_rotation();
        const float s = 0.5f + (zox_tst_maths_rand() + 1);
        const float3 v = zox_tst_maths_rand3(10);
        // turned, scaled, moved
        const float3 expected = float3_add(float3_scale(float4_rotate_float3(q, v), s), p);
        zox_tst_maths_track(&moves, zox_tst_maths_error3(float4x4_multiply_float3(float4x4_transform_scale(p, q, s), v), expected), i);
        zox_tst_maths_track(&rotation_only, zox_tst_maths_error3(float4x4_multiply_float3(float4x4_rotation(q), v), float4_rotate_float3(q, v)), i);
        zox_tst_maths_track(&position, zox_tst_maths_error3(float4x4_get_position(float4x4_transform_scale(p, q, s)), p), i);
        // composing
        const float4x4 a = zox_tst_matrix_rand_transform(1);
        const float4x4 b = zox_tst_matrix_rand_transform(1);
        const float3 ab_v = float4x4_multiply_float3(float4x4_multiply(a, b), v);
        const float3 a_b_v = float4x4_multiply_float3(b, float4x4_multiply_float3(a, v));
        // relative to its size: the points reach a few hundred
        zox_tst_maths_track(&composes, zox_tst_maths_error3(ab_v, a_b_v) / (1 + float3_length(a_b_v)), i);
        zox_tst_maths_track(&identity, zox_tst_matrix_error(float4x4_multiply(a, one), a), i);
        // undoing
        const float4x4 r = zox_tst_matrix_rand_transform(0);
        zox_tst_maths_track(&inverse, zox_tst_matrix_error(float4x4_multiply(float4x4_inverse(r), r), one), i);
        zox_tst_maths_track(&inverse_scaled, zox_tst_matrix_error(float4x4_multiply(float4x4_inverse(a), a), one), i);
        zox_tst_maths_track(&inverse_safe, zox_tst_matrix_error(float4x4_inverse_safe(a), float4x4_inverse(a)), i);
        zox_tst_maths_track(&transpose, zox_tst_matrix_error(float4x4_transpose(float4x4_transpose(a)), a), i);
    }
    zox_tst_maths_expect(moves, 1e-3f, "float4x4_transform_scale turns, scales, then moves a point");
    zox_tst_maths_expect(rotation_only, 1e-4f, "float4x4_rotation turns as the quaternion does");
    zox_tst_maths_expect(position, 1e-4f, "float4x4_get_position reads back the position");
    zox_tst_maths_expect(composes, 1e-4f, "float4x4_multiply(a, b) is a then b: (a b) v = b (a v)");
    zox_tst_maths_expect(identity, 0, "float4x4_multiply by identity changes nothing");
    zox_tst_maths_expect(inverse, 1e-3f, "float4x4_inverse undoes a transform");
    zox_tst_maths_expect(inverse_scaled, 1e-3f, "float4x4_inverse undoes a scaled transform");
    zox_tst_maths_expect(inverse_safe, 1e-3f, "float4x4_inverse_safe agrees with float4x4_inverse");
    zox_tst_maths_expect(transpose, 0, "float4x4_transpose twice is the matrix");
}
