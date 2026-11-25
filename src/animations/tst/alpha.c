// tst: animation_alpha, delayed alpha lerps and smooth fades hold their endpoints outside the active window.
static void zox_tst_animation_alpha_check(ecs* world) {
    const entity delayed = zox_new();
    const entity fading = zox_new();
    const double start = 100;
    zox_setv(delayed, AnimationState, zox_animate_alpha);
    zox_setv(delayed, AnimationStart, start);
    zox_setv(delayed, AnimationLength, 4);
    zox_setv(delayed, AnimationDelay, 2);
    zox_setv(delayed, AnimateSourceFloat, 0.25f);
    zox_setv(delayed, AnimateTargetFloat, 0.75f);
    zox_setv(delayed, Alpha, 0);
    zox_setv(fading, AnimationStart, start + 2);
    zox_setv(fading, FadeOutTime, 4);
    zox_setv(fading, Alpha, 0);
    const double samples[] = { 1, 2, 3, 4, 5, 6, 9 };
    const float delayed_expected[] = { 0.25f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.75f };
    const float fade_expected[] = { 1, 1, 0.84375f, 0.5f, 0.15625f, 0, 0 };
    for (int i = 0; i < 7; i++) {
        zox_tst_animation_run_at(world, zox_id(animate_alpha_system), start + samples[i]);
        zox_tst_animation_run_at(world, zox_id(fadeout_system), start + samples[i]);
        const float a = zox_getv(delayed, Alpha);
        const float b = zox_getv(fading, Alpha);
        zox_tst_check(fabsf(a - delayed_expected[i]) < 1e-5f, "delayed alpha at [%.1f] was [%.5f], expected [%.5f]",
            samples[i], a, delayed_expected[i]);
        zox_tst_check(fabsf(b - fade_expected[i]) < 1e-5f, "fade alpha at [%.1f] was [%.5f], expected [%.5f]",
            samples[i], b, fade_expected[i]);
    }
    zox_setv(delayed, AnimationState, zox_animation_waiting);
    zox_setv(delayed, Alpha, 0.125f);
    zox_tst_animation_run_at(world, zox_id(animate_alpha_system), start + 10);
    zox_tst_check(zox_getv(delayed, Alpha) == 0.125f, "a waiting animation changed alpha");
}
