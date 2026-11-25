// tst: particle_lifetime, changing DestroyInTime restarts a particle's fade and extends its expiration.
static byte zox_tst_particle_lifetime_ready(ecs* world) {
    static entity particle;
    static int step;
    static double initial_start, restart_time;
    static float previous_alpha = 1;
    const float initial_life = 0.7f;
    const float new_life = 0.85f;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            if (!zox_valid(prefab_particle3D)) {
                return 0;
            }
            particle = spawn_particle3(world, (float3) { 500, 500, 500 }, float3_zero, color_white);
            zox_setv(particle, DestroyInTime, initial_life);
            initial_start = zox_current_time;
            zox_tst_check(fabs(zox_getv(particle, FadeOutTime) - initial_life) < 1e-6 &&
                zox_getv(particle, AnimationStart) == initial_start, "initial life did not start the fade");
            break;
        case 1: {
            zox_tst_waiting = "the particle to fade partway";
            if (!zox_valid(particle)) {
                zox_tst_check(0, "the particle died before its partial fade could be observed");
                return 1;
            }
            const float alpha = zox_getv(particle, Alpha);
            if (alpha > 0.65f) {
                return 0;
            }
            zox_tst_check(alpha >= 0 && zox_getv(particle, AnimationStart) == initial_start,
                "lifetime countdown restarted the fade or alpha left its range");
            restart_time = zox_current_time;
            zox_setv(particle, DestroyInTime, new_life);
            zox_tst_check(fabs(zox_getv(particle, FadeOutTime) - new_life) < 1e-6 &&
                zox_getv(particle, AnimationStart) == restart_time, "changing the particle's life did not restart the fade");
            break;
        }
        case 2:
            zox_tst_waiting = "the restarted fade to return to high opacity";
            if (!zox_valid(particle)) {
                zox_tst_check(0, "the particle expired immediately after restarting");
                return 1;
            }
            previous_alpha = zox_getv(particle, Alpha);
            zox_tst_check(previous_alpha > 0.85f, "the restarted fade began at alpha [%.3f]", previous_alpha);
            break;
        default:
            zox_tst_waiting = "the renewed particle to expire";
            if (zox_valid(particle)) {
                const float alpha = zox_getv(particle, Alpha);
                if (alpha > previous_alpha || alpha < 0 || zox_getv(particle, AnimationStart) != restart_time) {
                    zox_tst_check(0, "the renewed particle's fade reversed or restarted during its countdown");
                    return 1;
                }
                previous_alpha = alpha;
                return 0;
            }
            zox_tst_check(zox_current_time > initial_start + initial_life && zox_current_time >= restart_time + new_life - 0.1,
                "the renewed particle expired early");
            zox_tst_check(previous_alpha < 0.35f, "the renewed particle was still opaque [%.3f] before expiration", previous_alpha);
            return 1;
    }
    step++;
    return 0;
}

static void zox_tst_particle_lifetime_check(ecs* world) {
    (void) world;
}
