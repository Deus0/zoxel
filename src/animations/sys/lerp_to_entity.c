void LerpToEntitySystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimationStartPosition);
    zox_sys_in(LerpToTarget);
    zox_sys_in(AnimationStart);
    zox_sys_in(AnimationDelay);
    zox_sys_in(AnimationLength);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(AnimationStartPosition, start_position);
        zox_sys_i(LerpToTarget, target);
        zox_sys_i(AnimationStart, start);
        zox_sys_i(AnimationDelay, delay);
        zox_sys_i(AnimationLength, length);
        zox_sys_o(Position3D, position);

        if (!zox_valid(target->value)) {
            continue;
        }

        zox_geter_value(target->value, Position3D, float3, end_position);

        double lerp = (zox_current_time - (start->value + delay->value)) / length->value;

        position->value = float3_lerp(start_position->value, end_position, lerp);

        // zox_log("lerp [%f] pos [%fx%fx%f]", lerp, position->value.x, position->value.y, position->value.z);
    }
} zoxd_system2(LerpToEntitySystem);