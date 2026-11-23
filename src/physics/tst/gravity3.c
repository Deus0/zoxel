// tst: physics3_gravity, 3D bodies under gravity in any direction (gravity3, acceleration3, friction3)
//  - 2s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants), no terrain
//  - gravity along each axis both ways, and two off any axis (one along (1, 1, 1), one tilted), 9.8 each:
//      falling: from rest, p = p0 + g t^2 / 2, v = g t, exactly
//      skydiving: Frictioned in the air (Grounded 0), heads for terminal velocity g / k along g
//  - and the frame by frame model (physics3.c) for every body
// tst: physics3_rolling_gravity, Rolling3D on a ground that's under the body's gravity, not under -y
//  - up is against its Gravity3D (DisableGravity on, so nothing pulls it: only its direction is used)
//  - bodies rolling with gravity +x, -z and +y (upside down), boxes of different half extents: the radius is the
//      half extent along up
//  - turned by distance / radius about up x v (world space), the contact point (radius along gravity) never slips
//  - no Gravity3D: up is +y (physics3_rotation's rolling bodies)
//  - run: bash bsh/tst.sh physics3_gravity (or physics3_rolling_gravity)

zox_tst_bodies3 zox_tst_p3g;
#define zox_tst_p3g_directions 8

static inline float3 zox_tst_p3g_direction(const int i) {
    const float3 directions[zox_tst_p3g_directions] = {
        { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 },
        { 1, 1, 1 }, { 0.3f, -0.8f, 0.52f } };
    return float3_scale(float3_normalize(directions[i]), -zox_default_gravity.y);
}

byte zox_tst_physics3_gravity_ready(ecs* world) {
    zox_tst_waiting = "3D bodies to fall";
    zox_tst_bodies3* set = &zox_tst_p3g;
    if (!set->spawned) {
        if (!zox_tst_clock_prepare(&set->clock, zox_tst_physics_dt)) {
            return 0;
        }
        for (int i = 0; i < zox_tst_p3g_directions; i++) {
            zox_tst_body3* b = zox_tst_bodies3_add(set, "falling");
            b->position = (float3) { i * 10.0f, 5, -3 };
            b->gravity = zox_tst_p3g_direction(i);
            b->has_gravity = 1;
            b = zox_tst_bodies3_add(set, "skydiving");
            b->position = (float3) { i * 10.0f, -5, 3 };
            b->gravity = zox_tst_p3g_direction(i);
            b->has_gravity = 1;
            b->frictioned = 1;
            b->has_grounded = 1;
        }
        zox_tst_bodies3_spawn(world, set);
        return 0;
    }
    zox_tst_bodies3_step(world, set);
    return zox_tst_physics_done(set->clock.time, zox_tst_physics_seconds);
}

void zox_tst_physics3_gravity_check(ecs* world) {
    const zox_tst_bodies3* set = &zox_tst_p3g;
    zox_tst_bodies3_compare(world, set, "physics3_gravity");
    const float t = (float) set->clock.time;
    const float k = air_friction3;
    for (int i = 0; i < set->count; i++) {
        const zox_tst_body3 s = set->start[i];
        const float3 g = s.gravity;
        const float3 p = zox_getv(set->entities[i], Position3D);
        const float3 v = zox_getv(set->entities[i], Velocity3D);
        if (!s.frictioned) {
            const float3 expected = float3_add(s.position, float3_scale(g, t * t / 2));
            zox_tst_check(zox_tst_float3_error(p, expected) < 1e-3f,
                "[falling %i] at [%f %f %f] expected p0 + g t^2 / 2 [%f %f %f]", i / 2, p.x, p.y, p.z, expected.x, expected.y, expected.z);
            zox_tst_check(zox_tst_float3_error(v, float3_scale(g, t)) < 1e-4f,
                "[falling %i] velocity [%f %f %f] expected g t", i / 2, v.x, v.y, v.z);
        } else {
            const float3 expected = float3_scale(g, (1 - expf(-k * t)) / k);
            zox_tst_check(zox_tst_float3_error(v, expected) < 1e-4f,
                "[skydiving %i] velocity [%f %f %f] expected (g / k) (1 - e^(-k t)) [%f %f %f]", i / 2,
                v.x, v.y, v.z, expected.x, expected.y, expected.z);
        }
    }
}

// rolling

typedef struct {
    float3 gravity;
    float3 velocity;
    float3 bounds;
} zox_tst_p3rg_body;

#define zox_tst_p3rg_count 3
const zox_tst_p3rg_body zox_tst_p3rg_bodies[zox_tst_p3rg_count] = {
    { { 9.8f, 0, 0 }, { 0, 1, 2 }, { 0.5f, 0.3f, 0.4f } },      // on a wall at +x, rolling up and along it
    { { 0, 0, -9.8f }, { 3, 0, 0 }, { 0.2f, 0.6f, 0.25f } },    // on a wall at -z
    { { 0, 9.8f, 0 }, { 0, 0, -2 }, { 0.3f, 0.35f, 0.3f } },    // upside down, on a ceiling
};
zox_tst_clock zox_tst_p3rg_clock;
entity zox_tst_p3rg_entities[zox_tst_p3rg_count];
float zox_tst_p3rg_worst_slip[zox_tst_p3rg_count];
byte zox_tst_p3rg_spawned = 0;

// up is against gravity, the radius is the half extent along it
static inline float3 zox_tst_p3rg_up(const zox_tst_p3rg_body* b) {
    return float3_scale(float3_normalize(b->gravity), -1);
}

static inline float zox_tst_p3rg_radius(const zox_tst_p3rg_body* b) {
    const float3 up = zox_tst_p3rg_up(b);
    return fabsf(up.x) * b->bounds.x + fabsf(up.y) * b->bounds.y + fabsf(up.z) * b->bounds.z;
}

byte zox_tst_physics3_rolling_gravity_ready(ecs* world) {
    zox_tst_waiting = "3D bodies to roll";
    if (!zox_tst_p3rg_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_p3rg_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_p3rg_spawned = 1;
        for (int i = 0; i < zox_tst_p3rg_count; i++) {
            const zox_tst_p3rg_body* b = &zox_tst_p3rg_bodies[i];
            const entity e = zox_new();
            zox_set(e, Position3D, { (float3) { i * 10.0f, 0, 0 } });
            zox_set(e, Velocity3D, { b->velocity });
            zox_set(e, Acceleration3D, { float3_zero });
            zox_set(e, Rotation3D, { quaternion_identity });
            zox_set(e, Omega3D, { float3_zero });
            zox_set(e, Alpha3D, { float3_zero });
            zox_set(e, Bounds3D, { b->bounds });
            zox_set(e, Grounded, { 1 });
            zox_set(e, Gravity3D, { b->gravity });
            zox_add(e, DisableGravity);
            zox_add(e, Rolling3D);
            zox_add(e, NoClip);
            zox_tst_p3rg_entities[i] = e;
        }
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_p3rg_clock);
    for (int i = 0; i < zox_tst_p3rg_count; i++) {
        const zox_tst_p3rg_body* b = &zox_tst_p3rg_bodies[i];
        const entity e = zox_tst_p3rg_entities[i];
        // contact point velocity: v + w x r, r from the centre to the ground (along gravity)
        const float3 omega = physics3_body_to_world(zox_getv(e, Rotation3D), zox_getv(e, Omega3D));
        const float3 down = float3_scale(zox_tst_p3rg_up(b), -zox_tst_p3rg_radius(b));
        const float3 contact = float3_add(zox_getv(e, Velocity3D), float3_cross(omega, down));
        // only the ground's plane: a speed along up isn't rolling
        const float3 up = zox_tst_p3rg_up(b);
        const float3 slide = float3_subtract(contact, float3_scale(up, float3_dot(contact, up)));
        const float slip = float3_length(slide);
        if (slip > zox_tst_p3rg_worst_slip[i]) {
            zox_tst_p3rg_worst_slip[i] = slip;
        }
    }
    return zox_tst_physics_done(zox_tst_p3rg_clock.time, zox_tst_physics_seconds);
}

void zox_tst_physics3_rolling_gravity_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_p3rg_clock, "physics3_rolling_gravity");
    const float t = (float) zox_tst_p3rg_clock.time;
    for (int i = 0; i < zox_tst_p3rg_count; i++) {
        const zox_tst_p3rg_body* b = &zox_tst_p3rg_bodies[i];
        const float3 up = zox_tst_p3rg_up(b);
        // rolls over the ground's plane: the velocity along it
        const float3 along = float3_subtract(b->velocity, float3_scale(up, float3_dot(b->velocity, up)));
        const float3 axis = float3_cross(up, along);
        const float angle = float3_length(along) * t / zox_tst_p3rg_radius(b);
        const float4 expected = zox_tst_axis_angle(axis, angle);
        const float4 q = zox_getv(zox_tst_p3rg_entities[i], Rotation3D);
        const float error = zox_tst_quaternion_error(q, expected);
        zox_tst_check(error <= 1e-3f, "[rolling %i] rotation [%f %f %f %f] expected [%f %f %f %f] (gravity [%.1f %.1f %.1f]), off by [%f]",
            i, q.x, q.y, q.z, q.w, expected.x, expected.y, expected.z, expected.w, b->gravity.x, b->gravity.y, b->gravity.z, error);
        zox_tst_check(zox_tst_p3rg_worst_slip[i] < 1e-3f, "[rolling %i] the contact point slid at [%f] (gravity [%.1f %.1f %.1f])",
            i, zox_tst_p3rg_worst_slip[i], b->gravity.x, b->gravity.y, b->gravity.z);
    }
}
