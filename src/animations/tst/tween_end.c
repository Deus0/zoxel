// tst: tween_end, a tween's end event (lerp_float_system: AnimationEnded, observed), for anything that waits on one
//  - tweens a float (AnimateSourceFloat) 0 to 1 on an entity, and one with AnimationDeletesTarget on another
//  - also covers a zero-duration tween and a tween whose target has already been deleted
//  - checks: successful tweens end once with the final value available to observers, then get removed
//  - run: bash bsh/tst.sh tween_end
int zox_tst_te2_stage = 0;
int zox_tst_te2_frames = 0;
enum { tween_kept_target, tween_deleted_target, tween_instant, tween_orphan, tween_fixture_count };
entity zox_tst_te2_targets[tween_fixture_count];
entity zox_tst_te2_tweens[tween_fixture_count];
int zox_tst_te2_ends[tween_fixture_count];
float zox_tst_te2_values_at_end[tween_fixture_count];

void zox_tst_tween_ended(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        for (int j = 0; j < tween_fixture_count; j++) {
            if (e == zox_tst_te2_tweens[j]) {
                zox_tst_te2_ends[j]++;
                if (zox_valid(zox_tst_te2_targets[j])) {
                    zox_tst_te2_values_at_end[j] = zox_getv(zox_tst_te2_targets[j], AnimateSourceFloat);
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
        for (int j = 0; j < tween_fixture_count; j++) {
            const entity e = zox_new();
            zox_setv(e, AnimateSourceFloat, 0);
            zox_tst_te2_targets[j] = e;
            zox_tst_te2_tweens[j] = zox_lerp_float_ins(e, AnimateSourceFloat, 0, 1, j == tween_instant ? 0 : 0.3);
        }
        zox_add(zox_tst_te2_tweens[tween_deleted_target], AnimationDeletesTarget);
        zox_delete(zox_tst_te2_targets[tween_orphan]);
        zox_tst_te2_stage = 1;
        zox_tst_te2_frames = 0;
        return 0;
    }
    if (zox_tst_te2_stage == 1 && zox_tst_te2_frames == 1) {
        zox_tst_check(zox_tst_te2_ends[tween_instant] == 1, "a zero-duration tween did not finish on its first update");
    }
    // Let disposal and event cleanup run after the successful tweens end.
    if (zox_tst_te2_stage == 1) {
        if (zox_tst_te2_ends[tween_kept_target] && zox_tst_te2_ends[tween_deleted_target]) {
            zox_tst_te2_stage = 2;
            zox_tst_te2_frames = 0;
        }
        return 0;
    }
    return zox_tst_te2_frames > 3;
}

void zox_tst_tween_end_check(ecs* world) {
    for (int i = 0; i < tween_fixture_count; i++) {
        zox_tst_check(zox_tst_te2_ends[i] == (i == tween_orphan ? 0 : 1), "tween [%i] emitted [%i] completion events", i, zox_tst_te2_ends[i]);
        zox_tst_check(!zox_valid(zox_tst_te2_tweens[i]), "tween [%i] was not cleaned up", i);
    }
    zox_tst_check(zox_tst_te2_values_at_end[tween_kept_target] == 1 && zox_tst_te2_values_at_end[tween_instant] == 1,
        "completion observers did not see the final value");
    zox_tst_check(zox_valid(zox_tst_te2_targets[tween_kept_target]) && zox_valid(zox_tst_te2_targets[tween_instant]),
        "a plain tween deleted its target");
    zox_tst_check(!zox_valid(zox_tst_te2_targets[tween_deleted_target]), "the AnimationDeletesTarget tween's target survived");
}
