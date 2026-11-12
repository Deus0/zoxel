// tst: physics3_motion, 3D linear motion (acceleration3, velocity3, friction3, gravity3, disabler, bounds)
//  - bodies, 2s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants):
//      falling: Gravity3D from rest, p = p0 - g t^2 / 2
//      drift: constant velocity
//      pushed: a force added every frame (like a controller), v = F t
//      sliding: Frictioned and Grounded, v = v0 e^(-k t), slides v0 (1 - e^(-k t)) / k, never reverses
//      running: Frictioned, Grounded and pushed, tops out at F / k at any frame rate
//      skydiving: Frictioned in the air with gravity, heads for terminal velocity g / k
//      spawning: InitializePhysics3D holds it still for its first physics_disable_frames, then
//          it falls from rest
//      no gravity: Gravity3D with DisableGravity doesn't fall
//      bounded: flies off fast, its whole box is held inside Position3DBounds
//  - exact steps (see physics/fun): the analytic answers hold at any frame rate, so these
//      check to float precision, a force landing a frame late (or a step error) fails them
//  - drift has no Acceleration3D (velocity3_system), the rest move in acceleration3 / friction3
// tst: physics3_rotation, 3D spin (alpha3, Omega3D, Dissipation3, Rolling3D)
//  - Omega3D is in the body's frame: rotation = rotation * delta
//  - exact steps, so spin up and damped spin match the analytic angle at any frame rate
//  - spin and damped have no Alpha3D (omega3_system / omega3_damped_system), the rest do (spin3)
//  - bodies: spin about y, a tilted body spinning about (1,2,3), spin up with Alpha3D,
//      Frictioned spin dying off as w0 e^(-rate t), a fast tumble staying a unit quaternion,
//      a tiny spin (below the system's threshold) not moving, and two Rolling3D bodies
//      (one turned first) rolling without slipping: the point touching the ground stays still
//  - run: bash bsh/tst.sh physics3_motion (or physics3_rotation)

typedef struct {
    float3 position;
    float3 velocity;
    float3 acceleration;    // forces built up, turned into velocity in zoxp_physics
    float3 force;           // added to Acceleration3D after every frame
    float3 gravity;
    float3 bounds;          // Bounds3D, half extents
    float6 area;            // Position3DBounds
    float4 rotation;
    float3 omega;
    float3 alpha;           // built up for the next frame
    float3 torque;          // added to Alpha3D after every frame
    byte grounded;
    byte has_grounded;
    byte frictioned;
    byte has_gravity;
    byte has_area;
    byte has_initialize;
    byte initialize;
    byte rolling;
    byte no_acceleration;   // no Acceleration3D, moves in velocity3_system
    byte no_alpha;          // no Alpha3D, spins in omega3_system / omega3_damped_system
} zox_tst_body3;

static inline zox_tst_body3 zox_tst_body3_new() {
    zox_tst_body3 b;
    memset(&b, 0, sizeof(b));
    b.rotation = quaternion_identity;
    return b;
}

// what the 3D systems do in one frame, in their pipeline order (all zoxp_physics)
static inline void zox_tst_body3_step(zox_tst_body3* b, const double delta_time) {
    const byte moves = delta_time <= zox_physics_max_delta_time;
    const float dt = (float) delta_time;
    // forces
    if (b->has_gravity && !b->no_acceleration) {
        b->acceleration = float3_add(b->acceleration, b->gravity);
    }
    if (b->has_initialize && b->initialize < physics_disable_frames) {
        b->initialize++;
        b->velocity = float3_zero;
        b->acceleration = float3_zero;
    }
    // linear step
    if (moves) {
        if (b->no_acceleration) {
            b->position = float3_add(b->position, float3_scale(b->velocity, delta_time));
        } else {
            const float k = b->frictioned && b->has_grounded ? (b->grounded ? ground_friction3 : air_friction3) : 0;
            b->position = float3_add(b->position, physics_step3(&b->velocity, b->acceleration, k, dt));
            b->acceleration = float3_zero;
        }
    }
    // spin
    const float c = b->frictioned ? omega_dissipation3 : 0;
    if (moves && b->rolling && b->has_grounded && b->grounded == 1) {
        const physics_step step = physics_step_new(c, dt);
        const float boost = step.g > 0 ? dt / step.g : 1;
        b->omega = float3_scale(physics3_world_to_body(b->rotation, physics3_rolling_omega(b->velocity, b->bounds.y)), boost);
    }
    if (moves) {
        const float3 turn = physics_step3(&b->omega, b->no_alpha ? float3_zero : b->alpha, c, dt);
        physics3_turn(&b->rotation, turn, dt);
        b->alpha = float3_zero;
    }
    if (b->has_area) {
        const float6 a = b->area;
        const float3 x = b->bounds;
        float3* p = &b->position;
        if (p->x - x.x < a.x) p->x = a.x + x.x + 0.2f; else if (p->x + x.x > a.y) p->x = a.y - x.x - 0.2f;
        if (p->y - x.y < a.z) p->y = a.z + x.y + 0.2f; else if (p->y + x.y > a.w) p->y = a.w - x.y - 0.2f;
        if (p->z - x.z < a.u) p->z = a.u + x.z + 0.2f; else if (p->z + x.z > a.v) p->z = a.v - x.z - 0.2f;
    }
    // what the test adds after the frame
    b->acceleration = float3_add(b->acceleration, b->force);
    b->alpha = float3_add(b->alpha, b->torque);
}

static inline entity zox_tst_body3_spawn(ecs* world, const zox_tst_body3* b) {
    const entity e = zox_new();
    zox_set(e, Position3D, { b->position });
    zox_set(e, Velocity3D, { b->velocity });
    if (!b->no_acceleration) {
        zox_set(e, Acceleration3D, { b->force });
    }
    zox_set(e, Rotation3D, { b->rotation });
    zox_set(e, Omega3D, { b->omega });
    if (!b->no_alpha) {
        zox_set(e, Alpha3D, { b->torque });
    }
    if (b->has_grounded) {
        zox_set(e, Grounded, { b->grounded });
    }
    if (b->frictioned) {
        zox_add(e, Frictioned);
    }
    if (b->has_gravity) {
        zox_set(e, Gravity3D, { b->gravity });
    }
    if (b->has_area || b->rolling) {
        zox_set(e, Bounds3D, { b->bounds });
    }
    if (b->has_area) {
        zox_set(e, Position3DBounds, { b->area });
    }
    if (b->has_initialize) {
        zox_set(e, InitializePhysics3D, { b->initialize });
    }
    if (b->rolling) {
        zox_add(e, Rolling3D);
        zox_add(e, NoClip);  // no terrain here, keeps terrain_collision_system off it
    }
    return e;
}

// the spawn values were the first frame's forces, after that they're added every frame
static inline void zox_tst_body3_first_forces(zox_tst_body3* b) {
    b->acceleration = b->force;
    b->alpha = b->torque;
}

static inline void zox_tst_body3_push(ecs* world, const entity e, const zox_tst_body3* b) {
    if (b->force.x != 0 || b->force.y != 0 || b->force.z != 0) {
        const float3 a = zox_getv(e, Acceleration3D);
        zox_set(e, Acceleration3D, { float3_add(a, b->force) });
    }
    if (b->torque.x != 0 || b->torque.y != 0 || b->torque.z != 0) {
        const float3 a = zox_getv(e, Alpha3D);
        zox_set(e, Alpha3D, { float3_add(a, b->torque) });
    }
}

static inline void zox_tst_body3_compare(ecs* world, const entity e, const zox_tst_body3* b, const char* label) {
    const float3 position = zox_getv(e, Position3D);
    const float3 velocity = zox_getv(e, Velocity3D);
    const float4 rotation = zox_getv(e, Rotation3D);
    const float3 omega = zox_getv(e, Omega3D);
    const float tolerance = 1e-4f;
    zox_tst_check(zox_tst_float3_finite(position) && zox_tst_float3_finite(velocity) && zox_tst_float4_finite(rotation),
        "[%s] went NaN or infinite", label);
    zox_tst_check(zox_tst_float3_error(position, b->position) <= tolerance * (1 + float3_length(b->position)),
        "[%s] position [%f %f %f] model [%f %f %f]", label,
        position.x, position.y, position.z, b->position.x, b->position.y, b->position.z);
    zox_tst_check(zox_tst_float3_error(velocity, b->velocity) <= tolerance * (1 + float3_length(b->velocity)),
        "[%s] velocity [%f %f %f] model [%f %f %f]", label,
        velocity.x, velocity.y, velocity.z, b->velocity.x, b->velocity.y, b->velocity.z);
    zox_tst_check(zox_tst_quaternion_error(rotation, b->rotation) <= 1e-3f,
        "[%s] rotation [%f %f %f %f] model [%f %f %f %f]", label,
        rotation.x, rotation.y, rotation.z, rotation.w, b->rotation.x, b->rotation.y, b->rotation.z, b->rotation.w);
    zox_tst_check(zox_tst_float3_error(omega, b->omega) <= tolerance * (1 + float3_length(b->omega)),
        "[%s] omega [%f %f %f] model [%f %f %f]", label,
        omega.x, omega.y, omega.z, b->omega.x, b->omega.y, b->omega.z);
}

// a set of bodies run together for zox_tst_physics_seconds
#define zox_tst_bodies3_max 16

typedef struct {
    zox_tst_clock clock;
    int count;
    zox_tst_body3 start[zox_tst_bodies3_max];
    zox_tst_body3 model[zox_tst_bodies3_max];
    entity entities[zox_tst_bodies3_max];
    const char* labels[zox_tst_bodies3_max];
    byte spawned;
} zox_tst_bodies3;

static inline zox_tst_body3* zox_tst_bodies3_add(zox_tst_bodies3* set, const char* label) {
    const int i = set->count++;
    set->labels[i] = label;
    set->start[i] = zox_tst_body3_new();
    return &set->start[i];
}

// after zox_tst_clock_prepare(&set->clock, dt)
static inline void zox_tst_bodies3_spawn(ecs* world, zox_tst_bodies3* set) {
    set->spawned = 1;
    for (int i = 0; i < set->count; i++) {
        set->model[i] = set->start[i];
        zox_tst_body3_first_forces(&set->model[i]);
        set->entities[i] = zox_tst_body3_spawn(world, &set->start[i]);
    }
}

static inline void zox_tst_bodies3_step(ecs* world, zox_tst_bodies3* set) {
    const double dt = zox_tst_clock_tick(&set->clock);
    for (int i = 0; i < set->count; i++) {
        zox_tst_body3_step(&set->model[i], dt);
        zox_tst_body3_push(world, set->entities[i], &set->start[i]);
    }
}

static inline void zox_tst_bodies3_compare(ecs* world, const zox_tst_bodies3* set, const char* name) {
    zox_tst_clock_check(&set->clock, name);
    for (int i = 0; i < set->count; i++) {
        zox_tst_body3_compare(world, set->entities[i], &set->model[i], set->labels[i]);
    }
}

// motion

zox_tst_bodies3 zox_tst_p3m;
int zox_tst_p3m_reversed = 0;   // frames the sliding body's velocity crossed zero
float3 zox_tst_p3m_spawn_held;  // the spawning body after its held frames
#define zox_tst_p3m_falling 0
#define zox_tst_p3m_drift 1
#define zox_tst_p3m_pushed 2
#define zox_tst_p3m_sliding 3
#define zox_tst_p3m_skydiving 4
#define zox_tst_p3m_spawning 5
#define zox_tst_p3m_no_gravity 6
#define zox_tst_p3m_bounded 7
#define zox_tst_p3m_running 8

byte zox_tst_physics3_motion_ready(ecs* world) {
    zox_tst_waiting = "3D bodies to move";
    zox_tst_bodies3* set = &zox_tst_p3m;
    if (!set->spawned) {
        if (!zox_tst_clock_prepare(&set->clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_body3* b = zox_tst_bodies3_add(set, "falling");
        b->position = (float3) { 0, 100, 0 };
        b->gravity = zox_default_gravity;
        b->has_gravity = 1;
        b = zox_tst_bodies3_add(set, "drift");
        b->position = (float3) { 1, 2, 3 };
        b->velocity = (float3) { 2, 0, -1 };
        b->no_acceleration = 1;
        b = zox_tst_bodies3_add(set, "pushed");
        b->force = (float3) { 0, 0, 3 };
        b = zox_tst_bodies3_add(set, "sliding");
        b->velocity = (float3) { 8, 0, -4 };
        b->frictioned = 1;
        b->has_grounded = 1;
        b->grounded = 1;
        b = zox_tst_bodies3_add(set, "skydiving");
        b->position = (float3) { 0, 200, 0 };
        b->gravity = zox_default_gravity;
        b->has_gravity = 1;
        b->frictioned = 1;
        b->has_grounded = 1;
        b = zox_tst_bodies3_add(set, "spawning");
        b->position = (float3) { 5, 50, 5 };
        b->velocity = (float3) { 5, 5, 0 };
        b->gravity = zox_default_gravity;
        b->has_gravity = 1;
        b->has_initialize = 1;
        b = zox_tst_bodies3_add(set, "no gravity");
        b->velocity = (float3) { 0, 0, 1 };
        b->gravity = zox_default_gravity;
        b->has_gravity = 1;
        b = zox_tst_bodies3_add(set, "bounded");
        b->velocity = (float3) { 40, -40, 0 };
        b->bounds = (float3) { 1, 1, 1 };
        b->area = (float6) { -5, 5, -5, 5, -5, 5 };
        b->has_area = 1;
        b = zox_tst_bodies3_add(set, "running");
        b->force = (float3) { 6, 0, 3 };
        b->frictioned = 1;
        b->has_grounded = 1;
        b->grounded = 1;
        zox_tst_bodies3_spawn(world, set);
        zox_add(set->entities[zox_tst_p3m_no_gravity], DisableGravity);
        set->model[zox_tst_p3m_no_gravity].has_gravity = 0;
        return 0;
    }
    zox_tst_bodies3_step(world, set);
    const float3 v = zox_getv(set->entities[zox_tst_p3m_sliding], Velocity3D);
    if (v.x < 0 || v.z > 0) {
        zox_tst_p3m_reversed++;
    }
    if (set->clock.frame == physics_disable_frames) {
        zox_tst_p3m_spawn_held = zox_getv(set->entities[zox_tst_p3m_spawning], Position3D);
    }
    return zox_tst_physics_done(set->clock.time, zox_tst_physics_seconds);
}

void zox_tst_physics3_motion_check(ecs* world) {
    const zox_tst_bodies3* set = &zox_tst_p3m;
    zox_tst_bodies3_compare(world, set, "physics3_motion");
    const float t = (float) set->clock.time;
    const float dt = (float) set->clock.dt;
    const float g = -zox_default_gravity.y;
    const entity* e = set->entities;
    const zox_tst_body3* s = set->start;
    // falling: v = -g t, p = p0 - g t^2 / 2, exactly
    {
        const float3 p = zox_getv(e[zox_tst_p3m_falling], Position3D);
        const float3 v = zox_getv(e[zox_tst_p3m_falling], Velocity3D);
        const float expected = s[zox_tst_p3m_falling].position.y - g * t * t / 2;
        zox_tst_check(zox_tst_near(p.y, expected, 1e-4f),
            "[falling] at y [%f] expected y0 - g t^2 / 2 [%f], off by [%f]", p.y, expected, p.y - expected);
        zox_tst_check(zox_tst_near(v.y, -g * t, 1e-4f),
            "[falling] velocity [%f] expected -g t [%f], gravity landing a frame late?", v.y, -g * t);
        zox_tst_check(p.x == 0 && p.z == 0, "[falling] drifted sideways [%f %f]", p.x, p.z);
    }
    // drift
    {
        const float3 p = zox_getv(e[zox_tst_p3m_drift], Position3D);
        const float3 expected = float3_add(s[zox_tst_p3m_drift].position, float3_scale(s[zox_tst_p3m_drift].velocity, t));
        zox_tst_check(zox_tst_float3_error(p, expected) < 1e-3f,
            "[drift] at [%f %f %f] expected [%f %f %f]", p.x, p.y, p.z, expected.x, expected.y, expected.z);
    }
    // pushed: v = F t, p = F t^2 / 2, exactly
    {
        const float3 F = s[zox_tst_p3m_pushed].force;
        const float3 v = zox_getv(e[zox_tst_p3m_pushed], Velocity3D);
        const float3 p = zox_getv(e[zox_tst_p3m_pushed], Position3D);
        zox_tst_check(zox_tst_near(v.z, F.z * t, 1e-4f), "[pushed] velocity [%f] expected F t [%f]", v.z, F.z * t);
        zox_tst_check(zox_tst_near(p.z, F.z * t * t / 2, 1e-4f),
            "[pushed] at [%f] expected F t^2 / 2 [%f]", p.z, F.z * t * t / 2);
    }
    // sliding: v = v0 e^(-k t), slides v0 (1 - e^(-k t)) / k, never reverses
    {
        const float k = ground_friction3;
        const float3 v0 = s[zox_tst_p3m_sliding].velocity;
        const float decay = expf(-k * t);
        const float3 v = zox_getv(e[zox_tst_p3m_sliding], Velocity3D);
        const float3 expected_v = float3_scale(v0, decay);
        zox_tst_check(zox_tst_float3_error(v, expected_v) <= 1e-4f * float3_length(v0),
            "[sliding] velocity [%f %f %f] expected v0 e^(-k t) [%f %f %f]", v.x, v.y, v.z, expected_v.x, expected_v.y, expected_v.z);
        zox_tst_check(!zox_tst_p3m_reversed, "[sliding] velocity reversed on [%i] frames, friction overshot", zox_tst_p3m_reversed);
        const float3 p = zox_getv(e[zox_tst_p3m_sliding], Position3D);
        const float3 distance = float3_scale(v0, (1 - decay) / k);
        zox_tst_check(zox_tst_float3_error(p, distance) <= 1e-4f * float3_length(v0) + 1e-4f,
            "[sliding] slid to [%f %f %f] expected v0 (1 - e^(-k t)) / k [%f %f %f]", p.x, p.y, p.z, distance.x, distance.y, distance.z);
    }
    // running: top speed F / k
    {
        const float3 F = s[zox_tst_p3m_running].force;
        const float3 v = zox_getv(e[zox_tst_p3m_running], Velocity3D);
        const float3 expected = float3_scale(F, 1 / ground_friction3);
        zox_tst_check(zox_tst_float3_error(v, expected) <= 1e-3f * float3_length(expected),
            "[running] top speed [%f %f %f] expected F / k [%f %f %f]", v.x, v.y, v.z, expected.x, expected.y, expected.z);
    }
    // skydiving: v = -(g / k) (1 - e^(-k t)), heading for terminal velocity, exactly
    {
        const float k = air_friction3;
        const float v = zox_getv(e[zox_tst_p3m_skydiving], Velocity3D).y;
        const float expected = -(g / k) * (1 - expf(-k * t));
        zox_tst_check(zox_tst_near(v, expected, 1e-4f),
            "[skydiving] velocity [%f] expected [%f], terminal [%f]", v, expected, -g / k);
    }
    // spawning: held still, then moves
    {
        const float3 p0 = s[zox_tst_p3m_spawning].position;
        zox_tst_check(zox_tst_float3_error(zox_tst_p3m_spawn_held, p0) < 1e-4f,
            "[spawning] moved to [%f %f %f] in its first [%i] frames, should be held at [%f %f %f]",
            zox_tst_p3m_spawn_held.x, zox_tst_p3m_spawn_held.y, zox_tst_p3m_spawn_held.z,
            physics_disable_frames, p0.x, p0.y, p0.z);
        const float3 p = zox_getv(e[zox_tst_p3m_spawning], Position3D);
        const float falling = t - physics_disable_frames * dt;
        zox_tst_check(zox_tst_near(p.y, p0.y - g * falling * falling / 2, 1e-4f),
            "[spawning] at y [%f] expected a fall from rest once let go [%f]", p.y, p0.y - g * falling * falling / 2);
        zox_tst_check(fabsf(p.x - p0.x) < 1e-4f, "[spawning] kept its spawn velocity, x [%f]", p.x);
    }
    // no gravity
    {
        const float3 v = zox_getv(e[zox_tst_p3m_no_gravity], Velocity3D);
        zox_tst_check(v.x == 0 && v.y == 0 && v.z == 1, "[no gravity] velocity [%f %f %f], DisableGravity was ignored", v.x, v.y, v.z);
    }
    // bounded: whole box inside
    {
        const zox_tst_body3 b = s[zox_tst_p3m_bounded];
        const float3 p = zox_getv(e[zox_tst_p3m_bounded], Position3D);
        zox_tst_check(p.x + b.bounds.x <= b.area.y && p.y - b.bounds.y >= b.area.z,
            "[bounded] box [%f %f] to [%f %f] pokes out of its bounds [%f %f] to [%f %f]",
            p.x - b.bounds.x, p.y - b.bounds.y, p.x + b.bounds.x, p.y + b.bounds.y,
            b.area.x, b.area.z, b.area.y, b.area.w);
    }
}

// rotation

zox_tst_bodies3 zox_tst_p3r;
float zox_tst_p3r_worst_length = 0; // worst |1 - |q|| seen, any body, any frame
float zox_tst_p3r_worst_slip = 0;   // fastest the rolling bodies' contact point moved
#define zox_tst_p3r_spin 0
#define zox_tst_p3r_tilted 1
#define zox_tst_p3r_spin_up 2
#define zox_tst_p3r_damped 3
#define zox_tst_p3r_tumble 4
#define zox_tst_p3r_tiny 5
#define zox_tst_p3r_rolling 6
#define zox_tst_p3r_rolling_turned 7

static inline float4 zox_tst_axis_angle(float3 axis, const float angle) {
    return quaternion_from_axis_angle(float3_normalize(axis), angle);
}

byte zox_tst_physics3_rotation_ready(ecs* world) {
    zox_tst_waiting = "3D bodies to spin";
    zox_tst_bodies3* set = &zox_tst_p3r;
    if (!set->spawned) {
        if (!zox_tst_clock_prepare(&set->clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_body3* b = zox_tst_bodies3_add(set, "spin");
        b->omega = (float3) { 0, 2, 0 };
        b->no_alpha = 1;
        b = zox_tst_bodies3_add(set, "tilted");
        b->rotation = zox_tst_axis_angle((float3) { 1, 0, 0 }, 0.7f);
        b->omega = (float3) { 1, 2, 3 };
        b = zox_tst_bodies3_add(set, "spin up");
        b->torque = (float3) { 0, 0, 1.5f };
        b = zox_tst_bodies3_add(set, "damped");
        b->omega = (float3) { 0, 5, 0 };
        b->frictioned = 1;
        b->no_alpha = 1;
        b = zox_tst_bodies3_add(set, "tumble");
        b->omega = (float3) { 7, -3, 11 };
        b = zox_tst_bodies3_add(set, "tiny");
        b->rotation = zox_tst_axis_angle((float3) { 0, 1, 1 }, 1.2f);
        b->omega = (float3) { 1e-8f, 0, 0 };
        b = zox_tst_bodies3_add(set, "rolling");
        b->velocity = (float3) { 3, 0, 0 };
        b->bounds = (float3) { 0.5f, 0.5f, 0.5f };
        b->rolling = 1;
        b->has_grounded = 1;
        b->grounded = 1;
        b = zox_tst_bodies3_add(set, "rolling turned");
        b->rotation = zox_tst_axis_angle((float3) { 0, 1, 0 }, (float) M_PI / 2);
        b->velocity = (float3) { 0, 0, 2 };
        b->bounds = (float3) { 0.25f, 0.25f, 0.25f };
        b->rolling = 1;
        b->has_grounded = 1;
        b->grounded = 1;
        zox_tst_bodies3_spawn(world, set);
        return 0;
    }
    zox_tst_bodies3_step(world, set);
    for (int i = 0; i < set->count; i++) {
        const float4 q = zox_getv(set->entities[i], Rotation3D);
        const float off = fabsf(1 - zox_tst_quaternion_length(q));
        if (off > zox_tst_p3r_worst_length || !isfinite(off)) {
            zox_tst_p3r_worst_length = off;
        }
        if (set->start[i].rolling) {
            // contact point velocity: v + w x r, r from the centre down to the ground
            const float4 rotation = zox_getv(set->entities[i], Rotation3D);
            const float3 omega = physics3_body_to_world(rotation, zox_getv(set->entities[i], Omega3D));
            const float3 down = { 0, -set->start[i].bounds.y, 0 };
            const float3 contact = float3_add(zox_getv(set->entities[i], Velocity3D), float3_cross(omega, down));
            const float slip = float3_length(contact);
            if (slip > zox_tst_p3r_worst_slip) {
                zox_tst_p3r_worst_slip = slip;
            }
        }
    }
    return zox_tst_physics_done(set->clock.time, zox_tst_physics_seconds);
}

static inline void zox_tst_p3r_expect(ecs* world, const int i, const float4 expected, const float tolerance) {
    const zox_tst_bodies3* set = &zox_tst_p3r;
    const float4 q = zox_getv(set->entities[i], Rotation3D);
    const float error = zox_tst_quaternion_error(q, expected);
    zox_tst_check(error <= tolerance,
        "[%s] rotation [%f %f %f %f] expected [%f %f %f %f], off by [%f]",
        set->labels[i], q.x, q.y, q.z, q.w, expected.x, expected.y, expected.z, expected.w, error);
}

void zox_tst_physics3_rotation_check(ecs* world) {
    const zox_tst_bodies3* set = &zox_tst_p3r;
    zox_tst_bodies3_compare(world, set, "physics3_rotation");
    const float t = (float) set->clock.time;
    const zox_tst_body3* s = set->start;
    zox_tst_check(zox_tst_p3r_worst_length < 1e-4f, "a rotation drifted from unit length by [%f]", zox_tst_p3r_worst_length);
    // maths: world <-> body round trips, and a true inverse (not just a normalize) for any length
    {
        const float4 q = s[zox_tst_p3r_tilted].rotation;
        const float3 w = { 1, 2, 3 };
        const float3 back = physics3_body_to_world(q, physics3_world_to_body(q, w));
        zox_tst_check(zox_tst_float3_error(back, w) < 1e-4f, "world to body and back gave [%f %f %f]", back.x, back.y, back.z);
        const float4 long_q = { q.x * 3, q.y * 3, q.z * 3, q.w * 3 };
        const float4 one = quaternion_rotate(long_q, quaternion_inverse(long_q));
        zox_tst_check(zox_tst_quaternion_error(one, quaternion_identity) < 1e-4f,
            "q * quaternion_inverse(q) gave [%f %f %f %f], not identity", one.x, one.y, one.z, one.w);
    }
    // constant spin: exactly w t about its axis, in the body's frame
    zox_tst_p3r_expect(world, zox_tst_p3r_spin, zox_tst_axis_angle(s[zox_tst_p3r_spin].omega, 2 * t), 1e-3f);
    const float3 tilted_w = s[zox_tst_p3r_tilted].omega;
    zox_tst_p3r_expect(world, zox_tst_p3r_tilted, quaternion_rotate(
        s[zox_tst_p3r_tilted].rotation,
        zox_tst_axis_angle(tilted_w, float3_length(tilted_w) * t)), 1e-3f);
    // spin up: w = alpha t, angle alpha t^2 / 2, exactly
    {
        const float alpha = s[zox_tst_p3r_spin_up].torque.z;
        const float w = zox_getv(set->entities[zox_tst_p3r_spin_up], Omega3D).z;
        zox_tst_check(zox_tst_near(w, alpha * t, 1e-4f), "[spin up] w [%f] expected alpha t [%f]", w, alpha * t);
        const float angle = alpha * t * t / 2;
        zox_tst_p3r_expect(world, zox_tst_p3r_spin_up, zox_tst_axis_angle((float3) { 0, 0, 1 }, angle), 1e-3f);
    }
    // damped: w0 e^(-rate t), turned by w0 (1 - e^(-rate t)) / rate, exactly
    {
        const float rate = omega_dissipation3;
        const float w0 = s[zox_tst_p3r_damped].omega.y;
        const float w = zox_getv(set->entities[zox_tst_p3r_damped], Omega3D).y;
        zox_tst_check(zox_tst_near(w, w0 * expf(-rate * t), 1e-3f), "[damped] w [%f] expected w0 e^(-rate t) [%f]", w, w0 * expf(-rate * t));
        const float angle = w0 * (1 - expf(-rate * t)) / rate;
        zox_tst_p3r_expect(world, zox_tst_p3r_damped, zox_tst_axis_angle((float3) { 0, 1, 0 }, angle), 1e-3f);
    }
    // tiny: below the threshold, untouched
    {
        const float4 q = zox_getv(set->entities[zox_tst_p3r_tiny], Rotation3D);
        const float4 q0 = s[zox_tst_p3r_tiny].rotation;
        zox_tst_check(q.x == q0.x && q.y == q0.y && q.z == q0.z && q.w == q0.w, "[tiny] a spin below the threshold moved it");
    }
    // rolling: turned by distance / radius about up x v, in world space, contact point still
    {
        const zox_tst_body3 b = s[zox_tst_p3r_rolling];
        const float angle = b.velocity.x * t / b.bounds.y;
        zox_tst_p3r_expect(world, zox_tst_p3r_rolling, zox_tst_axis_angle((float3) { 0, 0, -1 }, angle), 1e-3f);
        const zox_tst_body3 c = s[zox_tst_p3r_rolling_turned];
        const float angle2 = c.velocity.z * t / c.bounds.y;
        // world space spin goes on the left: turned first, then rolled about world x
        zox_tst_p3r_expect(world, zox_tst_p3r_rolling_turned,
            quaternion_rotate(zox_tst_axis_angle((float3) { 1, 0, 0 }, angle2), c.rotation), 1e-3f);
        zox_tst_check(zox_tst_p3r_worst_slip < 1e-3f, "[rolling] the contact point slid at [%f], it should roll without slipping", zox_tst_p3r_worst_slip);
    }
}
