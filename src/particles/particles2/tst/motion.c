// tst: particle2_motion, 2D particles integrate an impulse and spin, and respect configured bounds.
static zox_tst_clock zox_tst_particle2_motion_clock;
static entity zox_tst_particle2_moving, zox_tst_particle2_bounded;

static byte zox_tst_particle2_motion_ready(ecs* world) {
    zox_tst_clock* clock = &zox_tst_particle2_motion_clock;
    if (!zox_tst_particle2_moving) {
        if (!zox_tst_particle2_ready(world) || !zox_tst_clock_prepare(clock, 1.0 / 32.0)) {
            return 0;
        }
        zox_tst_waiting = "2D particle motion and bounds";
        zox_tst_particle2_moving = zox_ins(world, prefab_particle2);
        zox_setv(zox_tst_particle2_moving, Position2, ((float2) { 1, -1 }));
        zox_setv(zox_tst_particle2_moving, Velocity2D, ((float2) { 2, -1 }));
        zox_setv(zox_tst_particle2_moving, Acceleration2D, ((float2) { 4, 8 }));
        zox_setv(zox_tst_particle2_moving, Bounds2D, ((float2) { 20, 20 }));
        zox_setv(zox_tst_particle2_moving, Rotation2, 0.25f);
        zox_setv(zox_tst_particle2_moving, Torque2D, -3);

        zox_tst_particle2_bounded = zox_ins(world, prefab_particle2);
        zox_setv(zox_tst_particle2_bounded, Velocity2D, ((float2) { -8, 8 }));
        zox_setv(zox_tst_particle2_bounded, Bounds2D, ((float2) { 0.5f, 0.75f }));
        return 0;
    }
    zox_tst_clock_tick(clock);
    return clock->frame == 8;
}

static void zox_tst_particle2_motion_check(ecs* world) {
    const entity particle = zox_tst_particle2_moving;
    zox_tst_clock_check(&zox_tst_particle2_motion_clock, "particle2_motion");
    const float2 position = zox_getv(particle, Position2);
    const float2 velocity = zox_getv(particle, Velocity2D);
    // 8 steps at 1/32 s. The acceleration is consumed on the first step only.
    const float2 expected = { 1.529296875f, -1.19140625f };
    zox_tst_check(zox_tst_float2_error(position, expected) < 1e-5f, "particle position was [%.6f, %.6f]", position.x, position.y);
    zox_tst_check(float2_equals(velocity, ((float2) { 2.125f, -0.75f })) &&
        float2_equals(zox_getv(particle, Acceleration2D), float2_zero), "the particle's acceleration was not applied and cleared once");
    zox_tst_check(fabsf(zox_getv(particle, Rotation2) + 0.5f) < 1e-5f, "the particle did not spin by its angular velocity");
    zox_tst_check(float2_equals(zox_getv(zox_tst_particle2_bounded, Position2), ((float2) { -0.5f, 0.75f })),
        "the second particle did not stop at its configured bounds");
}
