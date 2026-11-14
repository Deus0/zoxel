// tst: tween_end, a tween's end event (lerp_float_system: AnimationEnded, observed), for anything that waits on one
//  - tweens a float (AnimateSourceFloat) 0 to 1 on an entity, and one with AnimationDeletesTarget on another
//  - checks: each ends once (its AnimationEnded observed), the first's target is 1 when it does; the frame after,
//      the tweens are gone, the first's target kept, the second's deleted with it
//  - run: bash bsh/tst.sh tween_end
int zox_tst_te2_stage = 0;
int zox_tst_te2_frames = 0;
entity zox_tst_te2_targets[2];
entity zox_tst_te2_tweens[2];
int zox_tst_te2_ends[2];
float zox_tst_te2_value_at_end = -1;

void zox_tst_tween_ended(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        for (int j = 0; j < 2; j++) {
            if (e == zox_tst_te2_tweens[j]) {
                zox_tst_te2_ends[j]++;
                if (j == 0) {
                    zox_tst_te2_value_at_end = zox_getv(zox_tst_te2_targets[0], AnimateSourceFloat);
                }
            }
        }
    }
}

byte zox_tst_tween_end_ready(ecs* world) {
    zox_tst_te2_frames++;
    zox_tst_progress(zox_tst_te2_frames);
    zox_tst_waiting = "the tweens to end";
    if (zox_tst_te2_stage == 0) {
        for (int j = 0; j < 2; j++) {
            const entity e = zox_new();
            zox_setv(e, AnimateSourceFloat, 0);
            zox_tst_te2_targets[j] = e;
            zox_tst_te2_tweens[j] = zox_lerp_float_ins(e, AnimateSourceFloat, 0, 1, 0.3);
        }
        zox_add(zox_tst_te2_tweens[1], AnimationDeletesTarget);
        zox_tst_te2_stage = 1;
        zox_tst_te2_frames = 0;
        return 0;
    }
    // both ended, then a few frames for them to be deleted
    if (zox_tst_te2_stage == 1) {
        if (zox_tst_te2_ends[0] && zox_tst_te2_ends[1]) {
            zox_tst_te2_stage = 2;
            zox_tst_te2_frames = 0;
        }
        return 0;
    }
    return zox_tst_te2_frames > 3;
}

void zox_tst_tween_end_check(ecs* world) {
    zox_tst_check(zox_tst_te2_ends[0] == 1 && zox_tst_te2_ends[1] == 1, "the tweens ended [%i] and [%i] times, not once",
        zox_tst_te2_ends[0], zox_tst_te2_ends[1]);
    zox_tst_check(zox_tst_te2_value_at_end == 1, "the target was [%.2f] when its tween ended, not 1", zox_tst_te2_value_at_end);
    zox_tst_check(!zox_valid(zox_tst_te2_tweens[0]) && !zox_valid(zox_tst_te2_tweens[1]), "the tweens weren't deleted after ending");
    zox_tst_check(zox_valid(zox_tst_te2_targets[0]), "the first tween's target was deleted");
    zox_tst_check(!zox_valid(zox_tst_te2_targets[1]), "the AnimationDeletesTarget tween's target wasn't deleted");
}

void zox_tsts_animations(ecs* world) {
    zox_tst_add("tween_end", zox_tst_tween_end_ready, zox_tst_tween_end_check, 20);
    zox_tst_set_setup("tween_end", NULL, 1);
    zox_on_add(zox_tst_tween_ended, [none] AnimationEnded);
}
