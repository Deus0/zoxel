// tst: physics2_perf / physics3_perf, cost of the physics systems per body per frame
//  - spawns zox_tst_physperf_bodies bodies with every physics component (forces, spin, friction)
//  - times the module's systems with the physics profiler (zox_physics_profiling): cpu time
//      inside them, summed over threads, so rendering and terrain streaming (running at the
//      same time with --autostart) don't count, the result is the cost on one core
//  - fails over a budget (ns per body per frame, all systems together), set loose for a debug
//      build on a slow arm board, it's there to catch something going badly wrong (a system
//      going O(n^2), locking per body), read the logged number when tuning
//  - also checks every body moved and none went NaN
//  - run: bash bsh/tst.sh physics3_perf (or physics2_perf)

#define zox_tst_physperf_bodies 50000
#define zox_tst_physperf_warmup 10
#define zox_tst_physperf_frames 60
#define zox_tst_physperf2_budget_ns 400.0     // measured ~150ns (-O0 debug, khadas arm, 2026-11-12)
#define zox_tst_physperf3_budget_ns 1250.0    // measured ~470ns (-O0 debug, khadas arm, 2026-11-12)

typedef struct {
    byte spawned;
    zox_tst_clock clock;
    double spawn_ms;
    double frame_start_ms;
    double frames_ms;
    unsigned long long ns;
    unsigned long long visits;
    entity* bodies;
} zox_tst_physperf;

zox_tst_physperf zox_tst_physperf2;
zox_tst_physperf zox_tst_physperf3;

uint zox_tst_physperf_seed = 777;

static inline float zox_tst_physperf_random() {
    uint x = zox_tst_physperf_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_physperf_seed = x;
    return (float) (x % 20001) / 10000.0f - 1.0f;  // -1 to 1
}

static inline float3 zox_tst_physperf_random3(const float scale) {
    return (float3) { zox_tst_physperf_random() * scale, zox_tst_physperf_random() * scale, zox_tst_physperf_random() * scale };
}

// warm up, then profile zox_tst_physperf_frames frames
static inline byte zox_tst_physperf_step(zox_tst_physperf* perf) {
    zox_tst_clock_tick(&perf->clock);
    const int frame = perf->clock.frame;
    if (frame == zox_tst_physperf_warmup) {
        zox_physics_profile_reset();
        zox_physics_profiling = 1;
        perf->frame_start_ms = get_time_ms();
    } else if (frame == zox_tst_physperf_warmup + zox_tst_physperf_frames) {
        zox_physics_profiling = 0;
        perf->frames_ms = get_time_ms() - perf->frame_start_ms;
        perf->ns = zox_physics_profile_ns;
        perf->visits = zox_physics_profile_bodies;
        return 1;
    }
    return 0;
}

static inline void zox_tst_physperf_report(zox_tst_physperf* perf, const char* name, const double budget_ns) {
    zox_tst_clock_check(&perf->clock, name);
    const double frames = zox_tst_physperf_frames;
    const double ms_per_frame = perf->ns / 1000000.0 / frames;
    const double ns_per_body = perf->ns / frames / zox_tst_physperf_bodies;
    const double visits_per_body = perf->visits / frames / zox_tst_physperf_bodies;
    zox_log("[tst] [%s] [%i] bodies: physics [%.3fms] cpu a frame = [%.1fns] a body ([%.1f] systems each, [%.1fns] a system), whole frame [%.1fms], spawned in [%.0fms], budget [%.0fns]",
        name, zox_tst_physperf_bodies, ms_per_frame, ns_per_body, visits_per_body,
        visits_per_body > 0 ? ns_per_body / visits_per_body : 0,
        perf->frames_ms / frames, perf->spawn_ms, budget_ns);
    zox_tst_check(visits_per_body >= 1, "[%s] the profiler saw no bodies, [%.2f] system visits a body a frame", name, visits_per_body);
    zox_tst_check(ns_per_body < budget_ns,
        "[%s] physics costs [%.1fns] a body a frame, over the [%.0fns] budget", name, ns_per_body, budget_ns);
}

// 2D

byte zox_tst_physics2_perf_ready(ecs* world) {
    zox_tst_waiting = "2D physics frames to time";
    zox_tst_physperf* perf = &zox_tst_physperf2;
    if (!perf->spawned) {
        if (!zox_tst_clock_prepare(&perf->clock, 1.0 / 60.0)) {
            return 0;
        }
        perf->spawned = 1;
        const double start = get_time_ms();
        perf->bodies = malloc(sizeof(entity) * zox_tst_physperf_bodies);
        for (int i = 0; i < zox_tst_physperf_bodies; i++) {
            const entity e = zox_new();
            zox_set(e, Position2, { { zox_tst_physperf_random() * 100, zox_tst_physperf_random() * 100 } });
            zox_set(e, Rotation2, { 0 });
            zox_set(e, Velocity2D, { { zox_tst_physperf_random() * 5, zox_tst_physperf_random() * 5 } });
            zox_set(e, Acceleration2D, { { 0, 0 } });
            zox_set(e, Torque2D, { zox_tst_physperf_random() * 3 });
            zox_set(e, Alpha2D, { 0 });
            zox_set(e, Gravity2D, { { 0, -9.8f } });
            zox_set(e, Bounds2D, { { 1000, 1000 } });
            zox_add(e, Frictioned);
            perf->bodies[i] = e;
        }
        perf->spawn_ms = get_time_ms() - start;
        return 0;
    }
    return zox_tst_physperf_step(perf);
}

void zox_tst_physics2_perf_check(ecs* world) {
    zox_tst_physperf* perf = &zox_tst_physperf2;
    zox_tst_physperf_report(perf, "physics2_perf", zox_tst_physperf2_budget_ns);
    int still = 0;
    int broken = 0;
    for (int i = 0; i < zox_tst_physperf_bodies; i++) {
        const float2 v = zox_getv(perf->bodies[i], Velocity2D);
        const float2 p = zox_getv(perf->bodies[i], Position2);
        if (!isfinite(p.x) || !isfinite(p.y) || !isfinite(v.x) || !isfinite(v.y)) {
            broken++;
        } else if (v.y >= 0) {
            still++; // gravity pulls them all down, friction keeps it under terminal velocity
        }
    }
    zox_tst_check(!broken, "[physics2_perf] [%i] bodies went NaN or infinite", broken);
    zox_tst_check(!still, "[physics2_perf] [%i] bodies aren't falling, physics didn't run on them", still);
}

// 3D

byte zox_tst_physics3_perf_ready(ecs* world) {
    zox_tst_waiting = "3D physics frames to time";
    zox_tst_physperf* perf = &zox_tst_physperf3;
    if (!perf->spawned) {
        if (!zox_tst_clock_prepare(&perf->clock, 1.0 / 60.0)) {
            return 0;
        }
        perf->spawned = 1;
        const double start = get_time_ms();
        perf->bodies = malloc(sizeof(entity) * zox_tst_physperf_bodies);
        for (int i = 0; i < zox_tst_physperf_bodies; i++) {
            const entity e = zox_new();
            zox_set(e, Position3D, { zox_tst_physperf_random3(100) });
            zox_set(e, Rotation3D, { quaternion_identity });
            zox_set(e, Velocity3D, { zox_tst_physperf_random3(5) });
            zox_set(e, Acceleration3D, { float3_zero });
            zox_set(e, Omega3D, { zox_tst_physperf_random3(3) });
            zox_set(e, Alpha3D, { float3_zero });
            zox_set(e, Gravity3D, { zox_default_gravity });
            zox_set(e, Grounded, { 0 });
            zox_add(e, Frictioned);
            perf->bodies[i] = e;
        }
        perf->spawn_ms = get_time_ms() - start;
        return 0;
    }
    return zox_tst_physperf_step(perf);
}

void zox_tst_physics3_perf_check(ecs* world) {
    zox_tst_physperf* perf = &zox_tst_physperf3;
    zox_tst_physperf_report(perf, "physics3_perf", zox_tst_physperf3_budget_ns);
    int still = 0;
    int broken = 0;
    for (int i = 0; i < zox_tst_physperf_bodies; i++) {
        const float3 v = zox_getv(perf->bodies[i], Velocity3D);
        const float3 p = zox_getv(perf->bodies[i], Position3D);
        const float4 q = zox_getv(perf->bodies[i], Rotation3D);
        if (!zox_tst_float3_finite(p) || !zox_tst_float3_finite(v) || !zox_tst_float4_finite(q)) {
            broken++;
        } else if (v.y >= 0 || q.w == 1) {
            still++; // all falling, all spun
        }
    }
    zox_tst_check(!broken, "[physics3_perf] [%i] bodies went NaN or infinite", broken);
    zox_tst_check(!still, "[physics3_perf] [%i] bodies didn't fall or spin, physics didn't run on them", still);
}
