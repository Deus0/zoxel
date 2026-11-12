// tst: physics2_motion, 2D linear motion (Friction2D, Gravity2D, Acceleration2D, Velocity2D, Bounds2D)
//  - bodies, 2s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants):
//      drift: constant velocity, p = p0 + v t
//      pushed: a force set every frame (like a controller), v = F t, p = F t^2 / 2
//      thrown: Gravity2D, a ball's arc, p = p0 + v0 t + g t^2 / 2
//      frictioned: slides to a stop, v = v0 e^(-k t), v0 / k away, never reverses
//      driven: Frictioned and pushed, tops out at F / k at any frame rate
//      bounded: flies off fast, held at its Bounds2D edge
//      no gravity: Gravity2D with DisableGravity doesn't fall
//  - each checked against a frame by frame model of the systems, and the analytic answer, which
//      the exact steps (see physics/fun) hit at any frame rate, not just within a step's error
//  - drift has no Acceleration2D (Velocity2DSystem), the rest move in Acceleration2D/Friction2D
// tst: physics2_rotation, 2D spin (Alpha2D, Torque2D, Rotation2)
//  - spin either way stays within (-2pi, 2pi) at the right angle, even faster than a turn a frame
//  - Alpha2D spins up, w = alpha t, angle = alpha t^2 / 2
//  - no spin leaves the angle alone
//  - run: bash bsh/tst.sh physics2_motion (or physics2_rotation)

typedef struct {
    float2 position;
    float2 velocity;
    float2 force;       // set into Acceleration2D after every frame
    float2 gravity;
    float2 bounds;
    float rotation;
    float torque;
    float alpha;        // set into Alpha2D after every frame
    byte frictioned;
    byte has_gravity;
    byte has_bounds;
    byte no_acceleration;   // no Acceleration2D, moves in Velocity2DSystem
    byte no_alpha;          // no Alpha2D, spins in Torque2DSystem
} zox_tst_body2;

// what the 2D systems do in one frame, in their order
static inline void zox_tst_body2_step(zox_tst_body2* b, const double delta_time) {
    const float dt = (float) delta_time;
    if (b->no_acceleration) {
        b->position.x += b->velocity.x * delta_time;
        b->position.y += b->velocity.y * delta_time;
    } else {
        float2 acceleration = b->force;
        if (b->has_gravity) {
            acceleration.x += b->gravity.x;
            acceleration.y += b->gravity.y;
        }
        const float2 step = physics_step2(&b->velocity, acceleration, b->frictioned ? friction2D : 0, dt);
        b->position.x += step.x;
        b->position.y += step.y;
    }
    if (b->no_alpha) {
        if (b->torque != 0) {
            b->rotation = physics2_wrap_angle(b->rotation + b->torque * delta_time);
        }
    } else {
        const float turn = physics_step1(&b->torque, b->alpha, 0, dt);
        if (turn != 0) {
            b->rotation = physics2_wrap_angle(b->rotation + turn);
        }
    }
    if (b->has_bounds) {
        b->position.x = b->position.x > b->bounds.x ? b->bounds.x : (b->position.x < -b->bounds.x ? -b->bounds.x : b->position.x);
        b->position.y = b->position.y > b->bounds.y ? b->bounds.y : (b->position.y < -b->bounds.y ? -b->bounds.y : b->position.y);
    }
}

static inline entity zox_tst_body2_spawn(ecs* world, const zox_tst_body2* b) {
    const entity e = zox_new();
    zox_set(e, Position2, { b->position });
    zox_set(e, Rotation2, { b->rotation });
    zox_set(e, Velocity2D, { b->velocity });
    if (!b->no_acceleration) {
        zox_set(e, Acceleration2D, { b->force });
    }
    zox_set(e, Torque2D, { b->torque });
    if (!b->no_alpha) {
        zox_set(e, Alpha2D, { b->alpha });
    }
    if (b->frictioned) {
        zox_add(e, Frictioned);
    }
    if (b->has_gravity) {
        zox_set(e, Gravity2D, { b->gravity });
    }
    if (b->has_bounds) {
        zox_set(e, Bounds2D, { b->bounds });
    }
    return e;
}

// forces are cleared by the systems every frame, set them again like a controller would
static inline void zox_tst_body2_push(ecs* world, const entity e, const zox_tst_body2* b) {
    if (b->force.x != 0 || b->force.y != 0) {
        zox_set(e, Acceleration2D, { b->force });
    }
    if (b->alpha != 0) {
        zox_set(e, Alpha2D, { b->alpha });
    }
}

// model vs entity, tight: same steps, only float rounding apart
static inline void zox_tst_body2_compare(ecs* world, const entity e, const zox_tst_body2* b, const char* label) {
    const float2 position = zox_getv(e, Position2);
    const float2 velocity = zox_getv(e, Velocity2D);
    const float rotation = zox_getv(e, Rotation2);
    const float tolerance = 1e-4f;
    zox_tst_check(zox_tst_float2_error(position, b->position) <= tolerance * (1 + zox_tst_float2_length(b->position)),
        "[%s] position [%f %f] model [%f %f]", label, position.x, position.y, b->position.x, b->position.y);
    zox_tst_check(zox_tst_float2_error(velocity, b->velocity) <= tolerance * (1 + zox_tst_float2_length(b->velocity)),
        "[%s] velocity [%f %f] model [%f %f]", label, velocity.x, velocity.y, b->velocity.x, b->velocity.y);
    zox_tst_check(zox_tst_angle_error(rotation, b->rotation) <= 1e-3,
        "[%s] rotation [%f] model [%f]", label, rotation, b->rotation);
}

// motion

#define zox_tst_p2m_drift 0
#define zox_tst_p2m_pushed 1
#define zox_tst_p2m_thrown 2
#define zox_tst_p2m_frictioned 3
#define zox_tst_p2m_bounded 4
#define zox_tst_p2m_no_gravity 5
#define zox_tst_p2m_driven 6
#define zox_tst_p2m_count 7

zox_tst_clock zox_tst_p2m_clock;
zox_tst_body2 zox_tst_p2m_bodies[zox_tst_p2m_count];
zox_tst_body2 zox_tst_p2m_start[zox_tst_p2m_count];
entity zox_tst_p2m_entities[zox_tst_p2m_count];
byte zox_tst_p2m_spawned = 0;
int zox_tst_p2m_reversed = 0;   // frames the frictioned body's velocity crossed zero

byte zox_tst_physics2_motion_ready(ecs* world) {
    zox_tst_waiting = "2D bodies to move";
    zox_tst_body2* b = zox_tst_p2m_bodies;
    if (!zox_tst_p2m_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_p2m_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_p2m_spawned = 1;
        memset(b, 0, sizeof(zox_tst_p2m_bodies));
        b[zox_tst_p2m_drift].position = (float2) { 1, 1 };
        b[zox_tst_p2m_drift].velocity = (float2) { 3, -2 };
        b[zox_tst_p2m_drift].no_acceleration = 1;
        b[zox_tst_p2m_pushed].force = (float2) { 2, -1 };
        b[zox_tst_p2m_thrown].position = (float2) { -4, 0 };
        b[zox_tst_p2m_thrown].velocity = (float2) { 4, 6 };
        b[zox_tst_p2m_thrown].gravity = (float2) { 0, -9.8f };
        b[zox_tst_p2m_thrown].has_gravity = 1;
        b[zox_tst_p2m_frictioned].velocity = (float2) { 10, 5 };
        b[zox_tst_p2m_frictioned].frictioned = 1;
        b[zox_tst_p2m_bounded].velocity = (float2) { 50, -30 };
        b[zox_tst_p2m_bounded].bounds = (float2) { 2, 1 };
        b[zox_tst_p2m_bounded].has_bounds = 1;
        b[zox_tst_p2m_no_gravity].velocity = (float2) { 1, 0 };
        b[zox_tst_p2m_no_gravity].gravity = (float2) { 0, -9.8f };
        b[zox_tst_p2m_no_gravity].has_gravity = 1;
        b[zox_tst_p2m_driven].force = (float2) { 16, -8 };
        b[zox_tst_p2m_driven].frictioned = 1;
        memcpy(zox_tst_p2m_start, b, sizeof(zox_tst_p2m_bodies));
        for (int i = 0; i < zox_tst_p2m_count; i++) {
            zox_tst_p2m_entities[i] = zox_tst_body2_spawn(world, &b[i]);
        }
        zox_add(zox_tst_p2m_entities[zox_tst_p2m_no_gravity], DisableGravity);
        // the model for the opted out body has no gravity
        b[zox_tst_p2m_no_gravity].has_gravity = 0;
        return 0;
    }
    const double dt = zox_tst_clock_tick(&zox_tst_p2m_clock);
    for (int i = 0; i < zox_tst_p2m_count; i++) {
        zox_tst_body2_step(&b[i], dt);
        zox_tst_body2_push(world, zox_tst_p2m_entities[i], &b[i]);
    }
    const float2 v = zox_getv(zox_tst_p2m_entities[zox_tst_p2m_frictioned], Velocity2D);
    if (v.x < 0 || v.y < 0) {
        zox_tst_p2m_reversed++;
    }
    return zox_tst_physics_done(zox_tst_p2m_clock.time, zox_tst_physics_seconds);
}

void zox_tst_physics2_motion_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_p2m_clock, "physics2_motion");
    const char* labels[] = { "drift", "pushed", "thrown", "frictioned", "bounded", "no gravity", "driven" };
    for (int i = 0; i < zox_tst_p2m_count; i++) {
        zox_tst_body2_compare(world, zox_tst_p2m_entities[i], &zox_tst_p2m_bodies[i], labels[i]);
    }
    const float t = (float) zox_tst_p2m_clock.time;
    const entity* e = zox_tst_p2m_entities;
    const zox_tst_body2* s = zox_tst_p2m_start;
    // drift
    {
        const float2 p = zox_getv(e[zox_tst_p2m_drift], Position2);
        const float2 expected = { s[0].position.x + s[0].velocity.x * t, s[0].position.y + s[0].velocity.y * t };
        zox_tst_check(zox_tst_float2_error(p, expected) < 1e-3f,
            "[drift] at [%f %f] expected [%f %f]", p.x, p.y, expected.x, expected.y);
    }
    // pushed: v = F t, p = F t^2 / 2, exactly
    {
        const float2 F = s[zox_tst_p2m_pushed].force;
        const float2 p = zox_getv(e[zox_tst_p2m_pushed], Position2);
        const float2 v = zox_getv(e[zox_tst_p2m_pushed], Velocity2D);
        const float2 expected_v = { F.x * t, F.y * t };
        const float2 expected_p = { F.x * t * t / 2, F.y * t * t / 2 };
        zox_tst_check(zox_tst_float2_error(v, expected_v) < 1e-3f,
            "[pushed] velocity [%f %f] expected F t [%f %f]", v.x, v.y, expected_v.x, expected_v.y);
        zox_tst_check(zox_tst_float2_error(p, expected_p) <= 1e-3f * (1 + zox_tst_float2_length(expected_p)),
            "[pushed] at [%f %f] expected F t^2 / 2 [%f %f]", p.x, p.y, expected_p.x, expected_p.y);
    }
    // thrown
    {
        const zox_tst_body2 b = s[zox_tst_p2m_thrown];
        const float2 p = zox_getv(e[zox_tst_p2m_thrown], Position2);
        const float2 v = zox_getv(e[zox_tst_p2m_thrown], Velocity2D);
        const float2 expected_p = {
            b.position.x + b.velocity.x * t + b.gravity.x * t * t / 2,
            b.position.y + b.velocity.y * t + b.gravity.y * t * t / 2 };
        const float2 expected_v = { b.velocity.x + b.gravity.x * t, b.velocity.y + b.gravity.y * t };
        zox_tst_check(zox_tst_float2_error(v, expected_v) < 1e-3f,
            "[thrown] velocity [%f %f] expected [%f %f]", v.x, v.y, expected_v.x, expected_v.y);
        zox_tst_check(zox_tst_float2_error(p, expected_p) <= 1e-3f * (1 + zox_tst_float2_length(expected_p)),
            "[thrown] at [%f %f] expected [%f %f]", p.x, p.y, expected_p.x, expected_p.y);
    }
    // frictioned: v = v0 e^(-k t), slid v0 (1 - e^(-k t)) / k, never reversed
    {
        const zox_tst_body2 b = s[zox_tst_p2m_frictioned];
        const float k = friction2D;
        const float decay = expf(-k * t);
        const float2 p = zox_getv(e[zox_tst_p2m_frictioned], Position2);
        const float2 v = zox_getv(e[zox_tst_p2m_frictioned], Velocity2D);
        const float2 expected_v = { b.velocity.x * decay, b.velocity.y * decay };
        const float2 expected_p = { b.velocity.x * (1 - decay) / k, b.velocity.y * (1 - decay) / k };
        zox_tst_check(zox_tst_float2_error(v, expected_v) <= 1e-4f,
            "[frictioned] velocity [%f %f] expected v0 e^(-kt) [%f %f]", v.x, v.y, expected_v.x, expected_v.y);
        zox_tst_check(zox_tst_float2_error(p, expected_p) <= 1e-3f * (1 + zox_tst_float2_length(expected_p)),
            "[frictioned] slid to [%f %f] expected v0 (1 - e^(-kt)) / k [%f %f]", p.x, p.y, expected_p.x, expected_p.y);
        zox_tst_check(!zox_tst_p2m_reversed,
            "[frictioned] velocity reversed on [%i] frames, friction overshot", zox_tst_p2m_reversed);
    }
    // driven: top speed F / k, the same at any frame rate
    {
        const float2 F = s[zox_tst_p2m_driven].force;
        const float2 v = zox_getv(e[zox_tst_p2m_driven], Velocity2D);
        const float2 expected = { F.x / friction2D, F.y / friction2D };
        zox_tst_check(zox_tst_float2_error(v, expected) <= 1e-3f * zox_tst_float2_length(expected),
            "[driven] top speed [%f %f] expected F / k [%f %f]", v.x, v.y, expected.x, expected.y);
    }
    // bounded
    {
        const zox_tst_body2 b = s[zox_tst_p2m_bounded];
        const float2 p = zox_getv(e[zox_tst_p2m_bounded], Position2);
        zox_tst_check(p.x == b.bounds.x && p.y == -b.bounds.y,
            "[bounded] at [%f %f] expected held at its edge [%f %f]", p.x, p.y, b.bounds.x, -b.bounds.y);
    }
    // no gravity
    {
        const float2 v = zox_getv(e[zox_tst_p2m_no_gravity], Velocity2D);
        zox_tst_check(v.x == 1 && v.y == 0, "[no gravity] velocity [%f %f], DisableGravity was ignored", v.x, v.y);
    }
}

// rotation

#define zox_tst_p2r_left 0
#define zox_tst_p2r_right 1
#define zox_tst_p2r_alpha 2
#define zox_tst_p2r_fast 3
#define zox_tst_p2r_still 4
#define zox_tst_p2r_count 5

zox_tst_clock zox_tst_p2r_clock;
zox_tst_body2 zox_tst_p2r_bodies[zox_tst_p2r_count];
entity zox_tst_p2r_entities[zox_tst_p2r_count];
byte zox_tst_p2r_spawned = 0;
float zox_tst_p2r_widest = 0;   // largest |angle| seen on any body, any frame

byte zox_tst_physics2_rotation_ready(ecs* world) {
    zox_tst_waiting = "2D bodies to spin";
    zox_tst_body2* b = zox_tst_p2r_bodies;
    if (!zox_tst_p2r_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_p2r_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_p2r_spawned = 1;
        memset(b, 0, sizeof(zox_tst_p2r_bodies));
        b[zox_tst_p2r_left].torque = 5;
        b[zox_tst_p2r_left].no_alpha = 1;
        b[zox_tst_p2r_right].torque = -5;
        b[zox_tst_p2r_alpha].alpha = 2;
        b[zox_tst_p2r_fast].torque = 1000;  // ~2.6 turns a frame
        b[zox_tst_p2r_fast].no_alpha = 1;
        b[zox_tst_p2r_still].rotation = -0.5f;
        for (int i = 0; i < zox_tst_p2r_count; i++) {
            zox_tst_p2r_entities[i] = zox_tst_body2_spawn(world, &b[i]);
        }
        return 0;
    }
    const double dt = zox_tst_clock_tick(&zox_tst_p2r_clock);
    for (int i = 0; i < zox_tst_p2r_count; i++) {
        zox_tst_body2_step(&b[i], dt);
        zox_tst_body2_push(world, zox_tst_p2r_entities[i], &b[i]);
        const float angle = fabsf(zox_getv(zox_tst_p2r_entities[i], Rotation2));
        if (angle > zox_tst_p2r_widest) {
            zox_tst_p2r_widest = angle;
        }
    }
    return zox_tst_physics_done(zox_tst_p2r_clock.time, zox_tst_physics_seconds);
}

void zox_tst_physics2_rotation_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_p2r_clock, "physics2_rotation");
    const char* labels[] = { "spin left", "spin right", "spin up", "spin fast", "still" };
    for (int i = 0; i < zox_tst_p2r_count; i++) {
        zox_tst_body2_compare(world, zox_tst_p2r_entities[i], &zox_tst_p2r_bodies[i], labels[i]);
    }
    zox_tst_check(zox_tst_p2r_widest < 2 * M_PI,
        "an angle reached [%f], it should stay within (-2pi, 2pi)", zox_tst_p2r_widest);
    const double t = zox_tst_p2r_clock.time;
    const entity* e = zox_tst_p2r_entities;
    const double left = zox_getv(e[zox_tst_p2r_left], Rotation2);
    const double right = zox_getv(e[zox_tst_p2r_right], Rotation2);
    const double fast = zox_getv(e[zox_tst_p2r_fast], Rotation2);
    const double still = zox_getv(e[zox_tst_p2r_still], Rotation2);
    zox_tst_check(zox_tst_angle_error(left, 5 * t) < 1e-3, "[spin left] at [%f] expected [%f]", left, remainder(5 * t, 2 * M_PI));
    zox_tst_check(zox_tst_angle_error(right, -5 * t) < 1e-3, "[spin right] at [%f] expected [%f]", right, remainder(-5 * t, 2 * M_PI));
    zox_tst_check(right < 0, "[spin right] at [%f], spinning right should keep a negative angle", right);
    zox_tst_check(zox_tst_angle_error(fast, 1000 * t) < 1e-2, "[spin fast] at [%f] expected [%f]", fast, remainder(1000 * t, 2 * M_PI));
    zox_tst_check(still == -0.5, "[still] moved to [%f]", still);
    // spin up: w = alpha t, angle = alpha t^2 / 2, exactly
    const float w = zox_getv(e[zox_tst_p2r_alpha], Torque2D);
    const double angle = zox_getv(e[zox_tst_p2r_alpha], Rotation2);
    zox_tst_check(zox_tst_near(w, (float) (2 * t), 1e-4f), "[spin up] w [%f] expected alpha t [%f]", w, 2 * t);
    zox_tst_check(zox_tst_angle_error(angle, t * t) <= 1e-3 * (1 + t * t),
        "[spin up] at [%f] expected alpha t^2 / 2 [%f]", angle, t * t);
    const float alpha = zox_getv(e[zox_tst_p2r_alpha], Alpha2D);
    zox_tst_check(alpha == 0 || alpha == 2, "[spin up] Alpha2D [%f] should be cleared each frame", alpha);
}
