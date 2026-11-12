// Shakes a targets LocalPosition3D for a duration, then returns it to its origin
// - if something else moves the target mid shake, that becomes the new origin
void shake_position_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimationTarget);
    zox_sys_in(AnimationStartTime);
    zox_sys_in(AnimationDuration);
    zox_sys_in(ShakeStrength);
    zox_sys_out(ShakeOrigin);
    zox_sys_out(ShakeOffset);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AnimationTarget, target);
        zox_sys_i(AnimationStartTime, start);
        zox_sys_i(AnimationDuration, duration);
        zox_sys_i(ShakeStrength, strength);
        zox_sys_o(ShakeOrigin, origin);
        zox_sys_o(ShakeOffset, offset);
        if (!zox_valid(target->value) || !zox_has(target->value, LocalPosition3D)) {
            zox_delete(e);
            continue;
        }
        zox_muter(target->value, LocalPosition3D, local);
        // moved by something else (ie camera mode change), accept that as the origin
        if (!float3_equals(local->value, float3_add(origin->value, offset->value))) {
            origin->value = local->value;
        }
        double time_passed = zox_current_time - start->value;
        if (time_passed >= duration->value) {
            local->value = origin->value;
            offset->value = float3_zero;
            zox_delete(e);
            continue;
        }
        // fades out over the duration
        float fade = duration->value > 0 ?
            1.0f - (float) (time_passed / duration->value) :
            0;
        float power = strength->value * fade;
        offset->value = (float3) {
            randf_range(-power, power),
            randf_range(-power, power),
            randf_range(-power, power)
        };
        local->value = float3_add(origin->value, offset->value);
    }
    zox_sys_on_end();
} zoxd_system(shake_position_system);
