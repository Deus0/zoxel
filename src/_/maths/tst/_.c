// tst: maths_vectors / maths_quaternions, pure logic tests for src/_/maths (no world, no gpu)
//  - known answers (right handed axes, 90 degree turns) and identities over many seeded random
//      inputs (composition, inverse, lengths kept), so a regression shows as a mismatch
//  - one check per property (not per sample), with the worst sample logged
//  - included from src/core/tst/_.c, as "_" is below the tst harness, ready on the first frame
//  - run: bash bsh/tst.sh maths_quaternions (or maths_vectors)

#define zox_tst_maths_samples 1000
#define zox_tst_maths_epsilon 1e-4f

uint zox_tst_maths_seed = 1234;

static inline float zox_tst_maths_rand() {
    uint x = zox_tst_maths_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_maths_seed = x;
    return (float) (x % 200001) / 100000.0f - 1.0f;  // -1 to 1
}

static inline float3 zox_tst_maths_rand3(const float scale) {
    return (float3) { zox_tst_maths_rand() * scale, zox_tst_maths_rand() * scale, zox_tst_maths_rand() * scale };
}

static inline float zox_tst_maths_error3(const float3 a, const float3 b) {
    return float3_length(float3_subtract(a, b));
}

// q and -q are the same rotation
static inline float zox_tst_maths_error4(const float4 a, const float4 b) {
    const float d1 = float4_length(float4_subtract(a, b));
    const float d2 = float4_length(float4_add(a, b));
    return d1 < d2 ? d1 : d2;
}

static inline float4 zox_tst_maths_rand_rotation() {
    float3 axis = zox_tst_maths_rand3(1);
    if (float3_length(axis) < 0.01f) {
        axis = (float3) { 0, 1, 0 };
    }
    return quaternion_from_axis_angle(float3_normalize(axis), zox_tst_maths_rand() * (float) M_PI);
}

// tracks the worst error of a property over its samples
typedef struct {
    float worst;
    int sample;
} zox_tst_maths_worst;

static inline void zox_tst_maths_track(zox_tst_maths_worst* w, const float error, const int sample) {
    if (!(error <= w->worst)) {  // NaN counts as worst
        w->worst = error;
        w->sample = sample;
    }
}

#define zox_tst_maths_expect(w, tolerance, label) \
    zox_tst_check((w).worst <= (tolerance), "[%s] off by [%f] (sample [%i])", label, (w).worst, (w).sample)

byte zox_tst_maths_ready(ecs* world) {
    (void) world;
    return 1;
}

// vectors

void zox_tst_maths_vectors_check(ecs* world) {
    (void) world;
    const float3 x = { 1, 0, 0 };
    const float3 y = { 0, 1, 0 };
    const float3 z = { 0, 0, 1 };
    // right handed: x cross y = z
    zox_tst_check(zox_tst_maths_error3(float3_cross(x, y), z) == 0, "x cross y isn't z, axes aren't right handed");
    zox_tst_check(zox_tst_maths_error3(float3_cross(y, z), x) == 0, "y cross z isn't x");
    zox_tst_check(zox_tst_maths_error3(float3_cross(z, x), y) == 0, "z cross x isn't y");
    zox_tst_check(float3_dot((float3) { 1, 2, 3 }, (float3) { 4, -5, 6 }) == 12, "dot of (1,2,3) (4,-5,6) isn't 12");
    zox_tst_check(float3_length((float3) { 3, 4, 12 }) == 13, "length of (3,4,12) isn't 13");
    zox_tst_check(float3_distance((float3) { 1, 1, 1 }, (float3) { 4, 5, 1 }) == 5, "distance (1,1,1) to (4,5,1) isn't 5");
    // normalize a zero vector: no NaN
    const float3 zero = float3_normalize(float3_zero);
    zox_tst_check(isfinite(zero.x) && isfinite(zero.y) && isfinite(zero.z), "normalizing a zero vector gave NaN");
    // lerp ends and middle
    const float3 a = { 1, -2, 3 };
    const float3 b = { 5, 6, -7 };
    zox_tst_check(zox_tst_maths_error3(float3_lerp(a, b, 0), a) == 0 && zox_tst_maths_error3(float3_lerp(a, b, 1), b) < 1e-6f,
        "float3_lerp doesn't hit its ends");
    zox_tst_check(zox_tst_maths_error3(float3_lerp(a, b, 0.5f), (float3) { 3, 2, -2 }) < 1e-6f, "float3_lerp middle is wrong");
    zox_tst_check(float_lerp(2, 6, 0.25f) == 3, "float_lerp(2, 6, 0.25) isn't 3");
    zox_tst_check(float_abs(-2.5f) == 2.5f && float_abs(2.5f) == 2.5f, "float_abs is wrong");
    zox_tst_check(float_max(2, 3) == 3 && float_min(2, 3) == 2, "float_max / float_min are wrong");
    // properties over random vectors
    zox_tst_maths_worst unit = { 0, 0 };
    zox_tst_maths_worst orthogonal = { 0, 0 };
    zox_tst_maths_worst cross_length = { 0, 0 };
    zox_tst_maths_worst normalize4 = { 0, 0 };
    for (int i = 0; i < zox_tst_maths_samples; i++) {
        const float3 u = zox_tst_maths_rand3(10);
        const float3 v = zox_tst_maths_rand3(10);
        if (float3_length(u) > 0.01f) {
            zox_tst_maths_track(&unit, fabsf(float3_length(float3_normalize(u)) - 1), i);
        }
        // u x v is perpendicular to both, |u x v| = |u| |v| sin(angle)
        const float3 c = float3_cross(u, v);
        const float scale = float3_length(u) * float3_length(v) + 1;
        zox_tst_maths_track(&orthogonal, (fabsf(float3_dot(c, u)) + fabsf(float3_dot(c, v))) / (scale * scale), i);
        const float d = float3_dot(u, v);
        const float lengths = float3_length(u) * float3_length(v);
        const float expected = sqrtf(fmaxf(0, lengths * lengths - d * d));
        zox_tst_maths_track(&cross_length, fabsf(float3_length(c) - expected) / scale, i);
        const float4 q = { zox_tst_maths_rand() * 5, zox_tst_maths_rand() * 5, zox_tst_maths_rand() * 5, zox_tst_maths_rand() * 5 + 6 };
        zox_tst_maths_track(&normalize4, fabsf(float4_length(float4_normalize(q)) - 1), i);
    }
    zox_tst_maths_expect(unit, zox_tst_maths_epsilon, "float3_normalize length");
    zox_tst_maths_expect(orthogonal, zox_tst_maths_epsilon, "float3_cross perpendicular");
    zox_tst_maths_expect(cross_length, 1e-3f, "float3_cross length");
    zox_tst_maths_expect(normalize4, zox_tst_maths_epsilon, "float4_normalize length");
}

// quaternions

void zox_tst_maths_quaternions_check(ecs* world) {
    (void) world;
    const float3 x = { 1, 0, 0 };
    const float3 y = { 0, 1, 0 };
    const float3 z = { 0, 0, 1 };
    const float half_turn = (float) M_PI / 2;
    // known answers: right handed, 90 degrees about y turns x to -z, z to x
    {
        const float4 q = quaternion_from_axis_angle(y, half_turn);
        const float3 a = float4_rotate_float3(q, x);
        const float3 b = float4_rotate_float3(q, z);
        zox_tst_check(zox_tst_maths_error3(a, (float3) { 0, 0, -1 }) < zox_tst_maths_epsilon,
            "90 degrees about y turned x to [%f %f %f], expected [0 0 -1]", a.x, a.y, a.z);
        zox_tst_check(zox_tst_maths_error3(b, x) < zox_tst_maths_epsilon,
            "90 degrees about y turned z to [%f %f %f], expected [1 0 0]", b.x, b.y, b.z);
        const float4 qx = quaternion_from_axis_angle(x, half_turn);
        const float3 c = float4_rotate_float3(qx, y);
        zox_tst_check(zox_tst_maths_error3(c, z) < zox_tst_maths_epsilon,
            "90 degrees about x turned y to [%f %f %f], expected [0 0 1]", c.x, c.y, c.z);
        const float3 d = float4_rotate_float3(quaternion_identity, (float3) { 1, 2, 3 });
        zox_tst_check(zox_tst_maths_error3(d, (float3) { 1, 2, 3 }) == 0, "identity moved a vector");
    }
    // bounds: turning a box a quarter about y swaps its x and z extents, 45 degrees mixes them
    {
        const float3 extents = { 1, 2, 3 };
        const float3 same = float4_rotate_bounds(quaternion_identity, extents);
        const float3 quarter = float4_rotate_bounds(quaternion_from_axis_angle(y, half_turn), extents);
        const float3 eighth = float4_rotate_bounds(quaternion_from_axis_angle(y, half_turn / 2), extents);
        const float mixed = (1 + 3) / sqrtf(2);
        zox_tst_check(zox_tst_maths_error3(same, extents) < zox_tst_maths_epsilon, "float4_rotate_bounds changed unrotated bounds");
        zox_tst_check(zox_tst_maths_error3(quarter, (float3) { 3, 2, 1 }) < zox_tst_maths_epsilon,
            "float4_rotate_bounds a quarter turn gave [%f %f %f], expected [3 2 1]", quarter.x, quarter.y, quarter.z);
        zox_tst_check(zox_tst_maths_error3(eighth, (float3) { mixed, 2, mixed }) < zox_tst_maths_epsilon,
            "float4_rotate_bounds 45 degrees gave [%f %f %f], expected [%f 2 %f]", eighth.x, eighth.y, eighth.z, mixed, mixed);
    }
    // inverse of a zero quaternion: no NaN
    {
        const float4 q = quaternion_inverse(float4_zero);
        zox_tst_check(isfinite(q.x) && isfinite(q.y) && isfinite(q.z) && isfinite(q.w), "quaternion_inverse of zero gave NaN");
    }
    zox_tst_maths_worst unit = { 0, 0 };
    zox_tst_maths_worst keeps_length = { 0, 0 };
    zox_tst_maths_worst keeps_angles = { 0, 0 };
    zox_tst_maths_worst composes = { 0, 0 };
    zox_tst_maths_worst associative = { 0, 0 };
    zox_tst_maths_worst inverse = { 0, 0 };
    zox_tst_maths_worst inverse_any_length = { 0, 0 };
    zox_tst_maths_worst conjugate = { 0, 0 };
    zox_tst_maths_worst angles_add = { 0, 0 };
    zox_tst_maths_worst axis_fixed = { 0, 0 };
    zox_tst_maths_worst euler = { 0, 0 };
    zox_tst_maths_worst between = { 0, 0 };
    zox_tst_maths_worst rotate_p = { 0, 0 };
    for (int i = 0; i < zox_tst_maths_samples; i++) {
        const float4 q1 = zox_tst_maths_rand_rotation();
        const float4 q2 = zox_tst_maths_rand_rotation();
        const float4 q3 = zox_tst_maths_rand_rotation();
        const float3 v = zox_tst_maths_rand3(10);
        const float3 u = zox_tst_maths_rand3(10);
        zox_tst_maths_track(&unit, fabsf(float4_length(q1) - 1), i);
        // rotating keeps lengths and the angles between vectors
        const float3 rv = float4_rotate_float3(q1, v);
        const float3 ru = float4_rotate_float3(q1, u);
        zox_tst_maths_track(&keeps_length, fabsf(float3_length(rv) - float3_length(v)) / (1 + float3_length(v)), i);
        zox_tst_maths_track(&keeps_angles, fabsf(float3_dot(ru, rv) - float3_dot(u, v)) / (1 + float3_length(u) * float3_length(v)), i);
        // (q1 q2) v = q1 (q2 v): q2 first
        const float3 composed = float4_rotate_float3(quaternion_rotate(q1, q2), v);
        const float3 in_turn = float4_rotate_float3(q1, float4_rotate_float3(q2, v));
        zox_tst_maths_track(&composes, zox_tst_maths_error3(composed, in_turn) / (1 + float3_length(v)), i);
        // (q1 q2) q3 = q1 (q2 q3)
        zox_tst_maths_track(&associative, zox_tst_maths_error4(
            quaternion_rotate(quaternion_rotate(q1, q2), q3),
            quaternion_rotate(q1, quaternion_rotate(q2, q3))), i);
        // q^-1 (q v) = v
        const float3 back = float4_rotate_float3(quaternion_inverse(q1), rv);
        zox_tst_maths_track(&inverse, zox_tst_maths_error3(back, v) / (1 + float3_length(v)), i);
        // q q^-1 = identity, at any length
        const float scale = 0.2f + fabsf(zox_tst_maths_rand()) * 5;
        const float4 long_q = float4_multiply_float(q1, scale);
        zox_tst_maths_track(&inverse_any_length, zox_tst_maths_error4(quaternion_rotate(long_q, quaternion_inverse(long_q)), quaternion_identity), i);
        // for a unit quaternion the inverse is the conjugate
        zox_tst_maths_track(&conjugate, zox_tst_maths_error4(float4_inverse(q1), quaternion_inverse(q1)), i);
        // turning a then b about one axis = turning a + b
        float3 axis = zox_tst_maths_rand3(1);
        if (float3_length(axis) < 0.01f) {
            axis = y;
        }
        axis = float3_normalize(axis);
        const float a = zox_tst_maths_rand() * 3;
        const float b = zox_tst_maths_rand() * 3;
        zox_tst_maths_track(&angles_add, zox_tst_maths_error4(
            quaternion_rotate(quaternion_from_axis_angle(axis, a), quaternion_from_axis_angle(axis, b)),
            quaternion_from_axis_angle(axis, a + b)), i);
        // the axis doesn't move when turned about itself
        zox_tst_maths_track(&axis_fixed, zox_tst_maths_error3(float4_rotate_float3(quaternion_from_axis_angle(axis, a), axis), axis), i);
        // euler round trip, the same rotation (away from the pitch singularity)
        const float3 angles = { zox_tst_maths_rand() * 3, zox_tst_maths_rand() * 1.3f, zox_tst_maths_rand() * 3 };
        const float4 qe = quaternion_from_euler(angles);
        const float4 qe2 = quaternion_from_euler(quaternion_to_euler(qe));
        zox_tst_maths_track(&euler, zox_tst_maths_error3(float4_rotate_float3(qe, v), float4_rotate_float3(qe2, v)) / (1 + float3_length(v)), i);
        // between vectors: turns u onto v's direction
        if (float3_length(u) > 0.1f && float3_length(v) > 0.1f && float3_dot(float3_normalize(u), float3_normalize(v)) > -0.99f) {
            const float4 qb = quaternion_from_between_vectors(u, v);
            const float3 turned = float3_normalize(float4_rotate_float3(qb, u));
            zox_tst_maths_track(&between, zox_tst_maths_error3(turned, float3_normalize(v)), i);
        }
        // the in place version matches
        float4 p = q1;
        quaternion_rotate_quaternion_p(&p, q2);
        zox_tst_maths_track(&rotate_p, zox_tst_maths_error4(p, quaternion_rotate(q1, q2)), i);
    }
    zox_tst_maths_expect(unit, zox_tst_maths_epsilon, "quaternion_from_axis_angle unit length");
    zox_tst_maths_expect(keeps_length, zox_tst_maths_epsilon, "float4_rotate_float3 keeps length");
    zox_tst_maths_expect(keeps_angles, 1e-3f, "float4_rotate_float3 keeps angles");
    zox_tst_maths_expect(composes, zox_tst_maths_epsilon, "quaternion_rotate composes (q1 q2) v = q1 (q2 v)");
    zox_tst_maths_expect(associative, zox_tst_maths_epsilon, "quaternion_rotate associative");
    zox_tst_maths_expect(inverse, zox_tst_maths_epsilon, "quaternion_inverse undoes a rotation");
    zox_tst_maths_expect(inverse_any_length, zox_tst_maths_epsilon, "quaternion_inverse q q^-1 = identity at any length");
    zox_tst_maths_expect(conjugate, zox_tst_maths_epsilon, "float4_inverse (conjugate) = quaternion_inverse for unit q");
    zox_tst_maths_expect(angles_add, zox_tst_maths_epsilon, "quaternion_from_axis_angle angles add");
    zox_tst_maths_expect(axis_fixed, zox_tst_maths_epsilon, "turning about an axis keeps the axis");
    zox_tst_maths_expect(euler, 1e-3f, "quaternion_from_euler(quaternion_to_euler(q)) round trip");
    zox_tst_maths_expect(between, 1e-3f, "quaternion_from_between_vectors turns u onto v");
    zox_tst_maths_expect(rotate_p, 0, "quaternion_rotate_quaternion_p matches quaternion_rotate");
}

void zox_tsts_maths() {
    zox_tst_add("maths_vectors", zox_tst_maths_ready, zox_tst_maths_vectors_check, 30);
    zox_tst_add("maths_quaternions", zox_tst_maths_ready, zox_tst_maths_quaternions_check, 30);
}
