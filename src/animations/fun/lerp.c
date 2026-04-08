void lerp_to_entity(ecs* world, entity e, entity target, float delay, float length) {
    zox_geter_value(e, Position3D, float3, position);
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, AnimationDelay, { delay });
    zox_set(e, AnimationLength, { length });
    zox_set(e, AnimationState, { zox_animate_position_entity });
    zox_set(e, AnimationPositionStart, { position });
    zox_set(e, LerpToTarget, { target });
}

void lerp_to_position(ecs* world, entity e, float delay, float length, float3 start, float3 end) {
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, AnimationDelay, { delay });
    zox_set(e, AnimationLength, { length });
    zox_set(e, AnimationState, { zox_animate_position });
    zox_set(e, AnimationPositionStart, { start });
    zox_set(e, AnimationPositionEnd, { end });
}
