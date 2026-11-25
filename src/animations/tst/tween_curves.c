// tst: tween_curves, float tweens sample each curve, including both back overshoots and clamped endpoints.
static void zox_tst_tween_curves_check(ecs* world) {
    const double start = 100;
    const float from = -2, to = 6;
    const byte curves[] = { zox_ease_linear, zox_ease_out, zox_ease_out_back, zox_ease_in_back, zox_ease_in_out };
    const float quarter[] = { 0.25f, 0.578125f, 0.817410f, -0.064137f, 0.15625f };
    const float half[] = { 0.5f, 0.875f, 1.0876975f, -0.0876975f, 0.5f };
    entity targets[5];
    for (int i = 0; i < 5; i++) {
        const entity target = zox_new();
        zox_setv(target, AnimateSourceFloat, 99);
        targets[i] = target;
        const entity tween = zox_lerp_float_ins(target, AnimateSourceFloat, from, to, 4);
        zox_setv(tween, AnimationStartTime, start);
        if (curves[i] != zox_ease_linear) {
            zox_setv(tween, AnimationEase, curves[i]);
        }
    }
    zox_tst_animation_run_at(world, zox_id(lerp_float_system), start - 1);
    for (int i = 0; i < 5; i++) {
        zox_tst_check(zox_getv(targets[i], AnimateSourceFloat) == from, "curve [%i] did not start at its lower clamp", i);
    }
    const float* fractions[] = { quarter, half };
    for (int sample = 0; sample < 2; sample++) {
        zox_tst_animation_run_at(world, zox_id(lerp_float_system), start + sample + 1);
        for (int i = 0; i < 5; i++) {
            const float expected = from + (to - from) * fractions[sample][i];
            const float actual = zox_getv(targets[i], AnimateSourceFloat);
            zox_tst_check(fabsf(actual - expected) < 0.001f, "curve [%i] at sample [%i] was [%.5f], expected [%.5f]", i, sample, actual, expected);
        }
    }
    zox_tst_animation_run_at(world, zox_id(lerp_float_system), start + 5);
    for (int i = 0; i < 5; i++) {
        zox_tst_check(zox_getv(targets[i], AnimateSourceFloat) == to, "curve [%i] did not finish at its upper clamp", i);
    }
}
