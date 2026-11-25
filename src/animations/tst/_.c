// Sample the actual systems at known times without advancing the rest of the world.
static void zox_tst_animation_run_at(ecs* world, entity system, double time) {
    const double previous = zox_current_time;
    zox_current_time = time;
    zox_run(world, system);
    zox_current_time = previous;
}

static byte zox_tst_animation_ready(ecs* world) {
    (void) world;
    return 1;
}

#include "tween_end.c"
#include "tween_curves.c"
#include "alpha.c"
#include "shake.c"

void zox_tsts_animations(ecs* world) {
    zox_tst_add("tween_end", zox_tst_tween_end_ready, zox_tst_tween_end_check, 20);
    zox_tst_set_setup("tween_end", NULL, 1);
    zox_on_add(zox_tst_tween_ended, [none] AnimationEnded);
    zox_tst_add("tween_curves", zox_tst_animation_ready, zox_tst_tween_curves_check, 20);
    zox_tst_set_setup("tween_curves", NULL, 1);
    zox_tst_add("animation_alpha", zox_tst_animation_ready, zox_tst_animation_alpha_check, 20);
    zox_tst_set_setup("animation_alpha", NULL, 1);
    zox_tst_add("animation_shake", zox_tst_animation_ready, zox_tst_animation_shake_check, 20);
    zox_tst_set_setup("animation_shake", NULL, 1);
}
