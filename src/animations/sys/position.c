zox_sys2(LerpToPositionSystem) {
    zox_sys_begin();
    zox_sys_in(AnimationState);
    zox_sys_in(AnimationPositionStart);
    zox_sys_in(AnimationPositionEnd);
    zox_sys_in(AnimationStart);
    zox_sys_in(AnimationDelay);
    zox_sys_in(AnimationLength);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(AnimationState, state);
        zox_sys_i(AnimationPositionStart, start_position);
        zox_sys_i(AnimationPositionEnd, end_position);
        zox_sys_i(AnimationStart, start);
        zox_sys_i(AnimationDelay, delay);
        zox_sys_i(AnimationLength, length);
        zox_sys_o(Position3D, position);

        if (state->value != zox_animate_position) {
            continue;
        }

        double lerp = (zox_current_time - (start->value + delay->value)) / length->value;

        position->value = float3_lerp(start_position->value, end_position->value, lerp);
    }
} zox_sys_end(LerpToPositionSystem);