// tst: particle2_lifetime, timed particles expire independently and zero life leaves an instance alive.
static zox_tst_clock zox_tst_particle2_life_clock;
static entity zox_tst_particle2_timed[2], zox_tst_particle2_persistent;

static byte zox_tst_particle2_lifetime_ready(ecs* world) {
    static const int expected_frames[] = { 4, 8 };
    zox_tst_clock* clock = &zox_tst_particle2_life_clock;
    if (!zox_tst_particle2_persistent) {
        if (!zox_tst_particle2_ready(world) || !zox_tst_clock_prepare(clock, 1.0 / 32.0)) {
            return 0;
        }
        zox_tst_waiting = "the 2D particle lifetimes to expire";
        zox_tst_particle2_persistent = zox_ins(world, prefab_particle2);
        for (int i = 0; i < 2; i++) {
            zox_tst_particle2_timed[i] = zox_ins(world, prefab_particle2);
            zox_setv(zox_tst_particle2_timed[i], DestroyInTime, expected_frames[i] * clock->dt);
        }
        return 0;
    }
    zox_tst_clock_tick(clock);
    for (int i = 0; i < 2; i++) {
        const byte alive = zox_valid(zox_tst_particle2_timed[i]);
        if (alive != (clock->frame < expected_frames[i])) {
            zox_tst_check(0, "timed particle [%i] had the wrong liveness at frame [%i], expires at [%i]",
                i, clock->frame, expected_frames[i]);
            return 1;
        }
    }
    return clock->frame == 9;
}

static void zox_tst_particle2_lifetime_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_particle2_life_clock, "particle2_lifetime");
    zox_tst_check(!zox_valid(zox_tst_particle2_timed[0]) && !zox_valid(zox_tst_particle2_timed[1]),
        "one or both timed particles still live");
    zox_tst_check(zox_valid(zox_tst_particle2_persistent) && zox_getv(zox_tst_particle2_persistent, DestroyInTime) == 0,
        "the particle with a zero lifetime did not persist");
}
