// shared by the physics tests (and src/terrain/collisions/tst)
//  - every frame of a test runs at a fixed delta time (zox_delta_time_override), so a test
//      knows exactly what the systems were given, and runs the same on any machine
//  - ready runs once after every world update, so each ready call after spawning = one frame
//  - two kinds of checks per body:
//      model: a frame by frame copy of what the systems should do, a tight match, catches
//          a system that's missing, runs twice, or in the wrong order
//      analytic: real physics (closed form answers), within the error a step of dt adds,
//          catches a model that's wrong in the same way the systems are

// the accuracy tests run for zox_tst_physics_seconds at zox_tst_physics_dt, 60fps unless a
// frame rate variant's setup (i.e. physics3_motion_10fps) changes it before boot
double zox_tst_physics_dt = 1.0 / 60.0;
#define zox_tst_physics_seconds 2.0

static inline byte zox_tst_physics_done(const double time, const double seconds) {
    return time >= seconds - 1e-6;
}

void zox_tst_physics_10fps() { zox_tst_physics_dt = 1.0 / 10.0; }
void zox_tst_physics_30fps() { zox_tst_physics_dt = 1.0 / 30.0; }
void zox_tst_physics_120fps() { zox_tst_physics_dt = 1.0 / 120.0; }

typedef struct {
    int frame;              // frames run since spawning
    double time;            // simulated seconds, the sum of those frames' dt
    double dt;              // the fixed dt asked for
    double next_dt;         // the dt the next frame will run at
    int dt_mismatches;      // frames that didn't run at it
} zox_tst_clock;

// NOTE: iterate_time_system runs at zoxp_end, so zox_delta_time read after a frame is the
//  dt the NEXT frame's physics will use (the one that just ran used the value read before)

static inline void zox_tst_clock_fix_dt(const double dt) {
    zox_delta_time_override = dt;
    zox_delta_time_override_count = 2;
}

// call before spawning, returns 1 once the next frame will run at the fixed dt (a frame later)
static inline byte zox_tst_clock_prepare(zox_tst_clock* clock, const double dt) {
    zox_tst_clock_fix_dt(dt);
    if (fabs(zox_delta_time - dt) > 1e-9) {
        return 0;
    }
    *clock = (zox_tst_clock) { 0, 0, dt, zox_delta_time, 0 };
    return 1;
}

// call once per ready after spawning: counts the frame that just ran, returns the dt it ran at
static inline double zox_tst_clock_tick(zox_tst_clock* clock) {
    const double dt = clock->next_dt;
    clock->next_dt = zox_delta_time;
    if (fabs(dt - clock->dt) > 1e-9) {
        clock->dt_mismatches++;
    }
    clock->frame++;
    clock->time += dt;
    zox_tst_clock_fix_dt(clock->dt);
    zox_tst_progress(clock->frame);
    return dt;
}

static inline void zox_tst_clock_check(const zox_tst_clock* clock, const char* label) {
    zox_log("[tst] [%s] ran [%i] frames at dt [%.4fs] ([%.0ffps]), [%.2fs]", label, clock->frame, clock->dt, 1.0 / clock->dt, clock->time);
    zox_tst_check(!clock->dt_mismatches,
        "[%s] [%i/%i] frames didn't run at the fixed dt [%f]",
        label,
        clock->dt_mismatches,
        clock->frame,
        clock->dt);
}

// relative-ish closeness, so big and small values get a fair tolerance
static inline byte zox_tst_near(const float a, const float b, const float tolerance) {
    return fabsf(a - b) <= tolerance * (1.0f + fabsf(b));
}

static inline float zox_tst_float2_length(const float2 a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

static inline float zox_tst_float2_error(const float2 a, const float2 b) {
    const float x = a.x - b.x;
    const float y = a.y - b.y;
    return sqrtf(x * x + y * y);
}

static inline float zox_tst_float3_error(const float3 a, const float3 b) {
    return float3_length(float3_subtract(a, b));
}

// q and -q are the same rotation
static inline float zox_tst_quaternion_error(const float4 a, const float4 b) {
    const float4 d1 = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    const float4 d2 = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    const float e1 = sqrtf(d1.x * d1.x + d1.y * d1.y + d1.z * d1.z + d1.w * d1.w);
    const float e2 = sqrtf(d2.x * d2.x + d2.y * d2.y + d2.z * d2.z + d2.w * d2.w);
    return e1 < e2 ? e1 : e2;
}

static inline float zox_tst_quaternion_length(const float4 q) {
    return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

// difference between two angles, ignoring whole turns, in [0, pi]
static inline double zox_tst_angle_error(const double a, const double b) {
    return fabs(remainder(a - b, 2 * M_PI));
}

static inline byte zox_tst_float3_finite(const float3 v) {
    return isfinite(v.x) && isfinite(v.y) && isfinite(v.z);
}

static inline byte zox_tst_float4_finite(const float4 v) {
    return isfinite(v.x) && isfinite(v.y) && isfinite(v.z) && isfinite(v.w);
}

static inline float zox_tst_median(double* values, const int count) {
    // insertion sort, tests sample a few hundred frames at most
    for (int i = 1; i < count; i++) {
        const double v = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > v) {
            values[j + 1] = values[j];
            j--;
        }
        values[j + 1] = v;
    }
    if (!count) {
        return 0;
    }
    return (float) values[count / 2];
}
