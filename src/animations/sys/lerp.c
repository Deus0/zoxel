// a simple lerp animation system: a float component of its target, eased (AnimationEase), it can delete its target
//  when done (AnimationDeletesTarget)
void lerp_float_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimationTarget);
    zox_sys_in(AnimationTargetComponent);
    zox_sys_in(AnimationStartTime);
    zox_sys_in(AnimationDuration);
    zox_sys_in(LerpFloatFrom);
    zox_sys_in(LerpFloatTo);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AnimationTarget, target);
        zox_sys_i(AnimationTargetComponent, id);
        zox_sys_i(AnimationStartTime, start);
        zox_sys_i(AnimationDuration, length);
        zox_sys_i(LerpFloatFrom, from);
        zox_sys_i(LerpFloatTo, to);
        if (!zox_valid(target->value)) {
            zox_delete(e);
            continue;
        }
        double time_passed = zox_current_time - start->value;
        double lerp_time = length->value > 0
            ? time_passed / length->value
            : 1;
        lerp_time = clampf(lerp_time, 0, 1);
        if (zox_has(e, AnimationEase)) {
            lerp_time = zox_ease(zox_getv(e, AnimationEase), lerp_time);
        }
        float new_value = float_lerp(from->value, to->value, lerp_time);
        zox_set_id(target->value, id->value, float, new_value);
        if (dbg_log) {
            zox_log("[%s]: TimePassed [%f:%f] Value [%f] [%f->%f]", zox_getn(target->value), time_passed, lerp_time, new_value, from->value, to->value);
        }
        if (time_passed >= length->value) {
            if (zox_has(e, AnimationDeletesTarget)) {
                zox_delete(target->value);
            }
            zox_delete(e);
        }
    }
    zox_sys_on_end();
} zoxd_system(lerp_float_system);
