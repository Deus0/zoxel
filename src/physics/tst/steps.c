// tst: physics_steps, the exact step (physics/fun physics_step1) on its own, no world
//  - one step of t lands where n steps of t / n do (frame rate independence, directly)
//  - matches the closed forms: v0 t + a t^2 / 2 without drag, terminal velocity a / k with it
//  - never overshoots, even with a huge drag * dt, and no NaN anywhere
//  - tiny drag behaves like no drag (the series keeps its precision)
//  - run: bash bsh/tst.sh physics_steps

uint zox_tst_steps_seed = 99;

static inline float zox_tst_steps_rand() {
    uint x = zox_tst_steps_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_steps_seed = x;
    return (float) (x % 200001) / 100000.0f - 1.0f;  // -1 to 1
}

byte zox_tst_physics_steps_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_physics_steps_check(ecs* world) {
    (void) world;
    const int splits[] = { 2, 3, 10, 60, 144 };
    float worst_split = 0;
    float worst_closed = 0;
    int overshoots = 0;
    int broken = 0;
    for (int i = 0; i < 500; i++) {
        const float v0 = zox_tst_steps_rand() * 20;
        const float a = zox_tst_steps_rand() * 15;
        const float k = i % 3 == 0 ? 0 : fabsf(zox_tst_steps_rand()) * 10;
        const float t = 0.05f + fabsf(zox_tst_steps_rand()) * 2;
        float v_one = v0;
        const float p_one = physics_step1(&v_one, a, k, t);
        for (int s = 0; s < 5; s++) {
            const int n = splits[s];
            float v = v0;
            float p = 0;
            for (int j = 0; j < n; j++) {
                p += physics_step1(&v, a, k, t / n);
            }
            const float scale = 1 + fabsf(p_one) + fabsf(v_one);
            const float error = (fabsf(p - p_one) + fabsf(v - v_one)) / scale;
            if (!(error <= worst_split)) {
                worst_split = error;
            }
        }
        // closed forms
        float expected_p;
        float expected_v;
        if (k == 0) {
            expected_v = v0 + a * t;
            expected_p = v0 * t + a * t * t / 2;
        } else {
            const float e = expf(-k * t);
            expected_v = a / k + (v0 - a / k) * e;
            expected_p = a / k * t + (v0 - a / k) * (1 - e) / k;
        }
        const float error = (fabsf(p_one - expected_p) + fabsf(v_one - expected_v)) / (1 + fabsf(expected_p) + fabsf(expected_v));
        if (!(error <= worst_closed)) {
            worst_closed = error;
        }
        // drag alone never reverses a velocity, at any step
        float v = v0;
        physics_step1(&v, 0, 1 + fabsf(zox_tst_steps_rand()) * 1000, 0.1f + fabsf(zox_tst_steps_rand()) * 10);
        if (v * v0 < 0) {
            overshoots++;
        }
        if (!isfinite(v) || !isfinite(p_one) || !isfinite(v_one)) {
            broken++;
        }
    }
    zox_tst_check(worst_split < 1e-4f, "one step and many smaller steps differ by [%f], not frame rate independent", worst_split);
    zox_tst_check(worst_closed < 1e-4f, "steps are off the closed form answers by [%f]", worst_closed);
    zox_tst_check(!overshoots, "drag reversed the velocity [%i] times", overshoots);
    zox_tst_check(!broken, "[%i] steps went NaN or infinite", broken);
    // tiny drag ~ no drag
    float v_tiny = 3;
    float v_none = 3;
    const float p_tiny = physics_step1(&v_tiny, -9.8f, 1e-6f, 1.0f / 120);
    const float p_none = physics_step1(&v_none, -9.8f, 0, 1.0f / 120);
    zox_tst_check(fabsf(p_tiny - p_none) < 1e-5f && fabsf(v_tiny - v_none) < 1e-4f,
        "a tiny drag step [%f %f] isn't close to no drag [%f %f]", p_tiny, v_tiny, p_none, v_none);
    // huge drag: settles at terminal velocity a / k
    float v_huge = 50;
    physics_step1(&v_huge, 10, 1000, 0.1f);
    zox_tst_check(fabsf(v_huge - 0.01f) < 1e-5f, "huge drag velocity [%f] isn't terminal a / k [0.01]", v_huge);
}
