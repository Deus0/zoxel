// tst: animation_shake, shaking follows external moves, refreshes in place, and cleans up its links.
static void zox_tst_shake_sample(ecs* world, entity target, entity shake, float3 origin, float max_offset) {
    zox_tst_check(zox_valid(shake), "shake expired before its sample");
    if (!zox_valid(shake)) {
        return;
    }
    const float3 offset = zox_getv(shake, ShakeOffset);
    const float3 position = zox_getv(target, LocalPosition3D);
    zox_tst_check(float3_equals(zox_getv(shake, ShakeOrigin), origin), "the shake kept the wrong origin");
    zox_tst_check(float3_equals(position, float3_add(origin, offset)), "the shake offset did not match the target position");
    zox_tst_check(fabsf(offset.x) <= max_offset && fabsf(offset.y) <= max_offset && fabsf(offset.z) <= max_offset,
        "shake offset [%.3f, %.3f, %.3f] exceeded its faded bound [%.3f]", offset.x, offset.y, offset.z, max_offset);
}

static void zox_tst_animation_shake_check(ecs* world) {
    const entity target = zox_new();
    zox_tst_check(!spawn_shake_position(world, target, 1, 2), "a target without LocalPosition3D started a shake");
    const float3 origin = { 3, 4, 5 };
    const float3 moved = { -7, 1, 9 };
    const double start = zox_current_time;
    zox_setv(target, LocalPosition3D, origin);
    const entity shake = spawn_shake_position(world, target, 2, 2);
    zox_tst_check(zox_valid(shake) && zox_get_link(world, target, ShakeLink) == shake, "the shake did not link to its target");
    if (!zox_valid(shake)) {
        return;
    }
    zox_tst_animation_run_at(world, zox_id(shake_position_system), start + 1);
    zox_tst_shake_sample(world, target, shake, origin, 1.00001f);
    zox_tst_check(!float3_equals(zox_getv(target, LocalPosition3D), origin), "the active shake did not move its target");

    zox_setv(target, LocalPosition3D, moved);
    zox_tst_animation_run_at(world, zox_id(shake_position_system), start + 1.25);
    zox_tst_shake_sample(world, target, shake, moved, 0.75001f);

    zox_current_time = start + 1.5;
    const entity refreshed = spawn_shake_position(world, target, 0.5f, 2);
    zox_current_time = start;
    zox_tst_check(refreshed == shake && zox_get_link(world, target, ShakeLink) == shake, "refreshing created a second shake");
    zox_tst_animation_run_at(world, zox_id(shake_position_system), start + 2.5);
    zox_tst_shake_sample(world, target, shake, moved, 0.25001f);
    zox_tst_animation_run_at(world, zox_id(shake_position_system), start + 3.75);
    zox_tst_check(float3_equals(zox_getv(target, LocalPosition3D), moved), "ending the shake did not restore the new origin");
    zox_tst_check(!zox_valid(shake) && !zox_get_link(world, target, ShakeLink), "ending the shake left a live entity or link");

    const entity disappearing = zox_new();
    zox_setv(disappearing, LocalPosition3D, origin);
    const entity orphan = spawn_shake_position(world, disappearing, 1, 2);
    zox_delete(disappearing);
    zox_tst_animation_run_at(world, zox_id(shake_position_system), start + 0.5);
    zox_tst_check(zox_valid(orphan) == 0, "deleting a target left its shake active");
}
