// tst: place_glance, placing a block the head turns part way to it (bones/sys/head_look.c, SwingTarget), then back
//  - a character with no camera on its head (characters/tst/character.c, held in the void), a slow place swing (2s)
//    at a point down and ahead
//  - in the swing its face (+z) points closer to the point than ahead does; after it, it looks ahead again
//  - run: bash bsh/tst.sh place_glance
byte zox_tst_pglance_stage = 0;
double zox_tst_pglance_at = 0;
float3 zox_tst_pglance_start;
float zox_tst_pglance_ahead = 0;     // ahead's facing on the point (a dot: 1 at it)
float zox_tst_pglance_turned = -1;   // the head's most facing on it in the swing
float zox_tst_pglance_after = 0;     // the head's local turn from ahead after it (radians)

static float3 zox_tst_pglance_point(ecs* world, entity head) {
    const float4 body = zox_getv(zox_tst_body, Rotation3D);
    return float3_add(zox_getv(head, Position3D), float4_rotate_float3(body, (float3) { 0.5f, -1.5f, 1.5f }));
}

byte zox_tst_place_glance_ready(ecs* world) {
    const entity e = zox_tst_body_ready(world, 0, (float3) { 0, 400, 0 });
    if (!e) {
        return 0;
    }
    if (!zox_tst_pglance_at) {
        zox_tst_pglance_start = zox_getv(e, Position3D);
        zox_tst_pglance_at = zox_current_time;
    }
    zox_setv(e, Position3D, zox_tst_pglance_start);
    zox_setv(e, Velocity3D, float3_zero);
    const entity skeleton = zox_get_link(world, e, SkeletonLink);
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const double t = zox_current_time - zox_tst_pglance_at;
    zox_tst_progress(zox_tst_pglance_stage * 1000 + t * 10);
    if (!zox_valid(head)) {
        zox_tst_waiting = "the character's head";
        return 0;
    }
    const float3 point = zox_tst_pglance_point(world, head);
    const float3 to = float3_normalize(float3_sub(point, zox_getv(head, Position3D)));
    const float facing = float3_dot(float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward), to);
    if (zox_tst_pglance_stage == 0) {
        zox_tst_waiting = "the body to settle";
        if (t > 1) {
            zox_tst_pglance_ahead = float3_dot(float4_rotate_float3(zox_getv(e, Rotation3D), float3_forward), to);
            zox_setv(skeleton, SwingStart, zox_current_time);
            zox_setv(skeleton, SwingSpeed, 2);
            zox_setv(skeleton, SwingKind, zox_swing_place);
            zox_setv(skeleton, SwingTarget, point);
            zox_tst_pglance_stage = 1;
        }
        return 0;
    }
    const double passed = zox_current_time - zox_getv(skeleton, SwingStart);
    if (zox_tst_pglance_stage == 1) {
        zox_tst_waiting = "the place";
        if (facing > zox_tst_pglance_turned) {
            zox_tst_pglance_turned = facing;
        }
        if (passed > 2) {
            zox_tst_pglance_stage = 2;
        }
        return 0;
    }
    zox_tst_waiting = "the head back ahead";
    if (passed < 3.5) {
        return 0;
    }
    const float d = fabsf(zox_getv(head, LocalRotation3D).w);
    zox_tst_pglance_after = 2 * acosf(d > 1 ? 1 : d);
    return 1;
}

void zox_tst_place_glance_check(ecs* world) {
    zox_log("[tst] place_glance: facing the point ahead [%.3f], turned [%.3f], after the turn left [%.3f] radians",
        zox_tst_pglance_ahead, zox_tst_pglance_turned, zox_tst_pglance_after);
    zox_tst_check(zox_tst_pglance_stage == 2, "the place never ended [%i]", zox_tst_pglance_stage);
    zox_tst_check(zox_tst_pglance_turned > zox_tst_pglance_ahead + 0.15f, "the head didn't turn to the place [%f of %f]", zox_tst_pglance_turned, zox_tst_pglance_ahead);
    zox_tst_check(zox_tst_pglance_after < 0.05f, "the head didn't look ahead again [%f]", zox_tst_pglance_after);
}

void zox_tsts_place_glance(ecs* world) {
    zox_tst_add("place_glance", zox_tst_place_glance_ready, zox_tst_place_glance_check, 120);
}
