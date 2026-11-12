// Exact steps: each frame solves dv/dt = a - k v exactly, for a force a held over the frame and
// a linear drag k (friction), so results don't depend on the frame rate at all:
//  - a fall: p = v0 t + g t^2 / 2, a jump peaks at v^2 / 2g
//  - pushed with drag: top speed F / k, sliding to a stop goes v0 / k, terminal velocity g / k
//  - never overshoots, at any dt
//  - the same for spin (omega, alpha, angular drag)
// the step as coefficients, the same for every axis (and every body with that drag and dt):
//  v' = v keep + a g,  moved = v g + a h
//  - keep = e^(-k dt), g = (1 - keep) / k, h = (dt - g) / k
//  - k dt small: a series, as 1 - keep and dt - g lose their precision to cancellation in floats
//      (and k = 0 gives keep 1, g dt, h dt^2 / 2: plain constant acceleration)
typedef struct {
    float keep;
    float g;
    float h;
} physics_step;

static inline physics_step physics_step_new(const float k, const float dt) {
    const double x = (double) k * dt;
    const double t = dt;
    if (x < 0.1) {
        // e^-x series, the error is ~x^5 / 720 of the step, under 1e-8
        const double x2 = x * x;
        const double x3 = x2 * x;
        const double x4 = x3 * x;
        return (physics_step) {
            (float) (1 - x + x2 / 2 - x3 / 6 + x4 / 24),
            (float) (t * (1 - x / 2 + x2 / 6 - x3 / 24 + x4 / 120)),
            (float) (t * t * (0.5 - x / 6 + x2 / 24 - x3 / 120 + x4 / 720)) };
    }
    const double keep = exp(-x);
    const double g = (1 - keep) / k;
    return (physics_step) { (float) keep, (float) g, (float) ((t - g) / k) };
}

// returns the distance moved over the step, updates the velocity
static inline float physics_step_apply(const physics_step step, float* velocity, const float a) {
    const float v = *velocity;
    *velocity = v * step.keep + a * step.g;
    return v * step.g + a * step.h;
}

static inline float2 physics_step_apply2(const physics_step step, float2* velocity, const float2 a) {
    return (float2) {
        physics_step_apply(step, &velocity->x, a.x),
        physics_step_apply(step, &velocity->y, a.y) };
}

static inline float3 physics_step_apply3(const physics_step step, float3* velocity, const float3 a) {
    return (float3) {
        physics_step_apply(step, &velocity->x, a.x),
        physics_step_apply(step, &velocity->y, a.y),
        physics_step_apply(step, &velocity->z, a.z) };
}

static inline float physics_step1(float* velocity, const float a, const float k, const float dt) {
    return physics_step_apply(physics_step_new(k, dt), velocity, a);
}

static inline float2 physics_step2(float2* velocity, const float2 a, const float k, const float dt) {
    const physics_step step = physics_step_new(k, dt);
    return (float2) {
        physics_step_apply(step, &velocity->x, a.x),
        physics_step_apply(step, &velocity->y, a.y) };
}

static inline float3 physics_step3(float3* velocity, const float3 a, const float k, const float dt) {
    const physics_step step = physics_step_new(k, dt);
    return (float3) {
        physics_step_apply(step, &velocity->x, a.x),
        physics_step_apply(step, &velocity->y, a.y),
        physics_step_apply(step, &velocity->z, a.z) };
}

// Profiling, off unless turned on (physics perf tests, debug ui)
//  - cpu time spent in the physics systems, summed over worker threads, so it's the cost
//      on one core, and how many bodies they went through
//  - timed per iteration (a table on a thread), never per body
byte zox_physics_profiling = 0;
unsigned long long zox_physics_profile_ns = 0;
unsigned long long zox_physics_profile_bodies = 0;

#define zox_physics_profile_begin()\
    const double physics_profile_start = zox_physics_profiling ? get_time_ms() : 0

#define zox_physics_profile_end()\
    if (zox_physics_profiling) {\
        __atomic_fetch_add(&zox_physics_profile_ns, (unsigned long long) ((get_time_ms() - physics_profile_start) * 1000000.0), __ATOMIC_RELAXED);\
        __atomic_fetch_add(&zox_physics_profile_bodies, (unsigned long long) it->count, __ATOMIC_RELAXED);\
    }

static inline void zox_physics_profile_reset() {
    __atomic_store_n(&zox_physics_profile_ns, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&zox_physics_profile_bodies, 0, __ATOMIC_RELAXED);
}
