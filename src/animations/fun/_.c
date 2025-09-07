void lerp_to_entity(
    ecs* world,
    entity e,
    entity target,
    float delay,
    float length
) {
    zox_geter_value(e, Position3D, float3, position);
    zox_set(e, AnimationStartPosition, { position });
    zox_set(e, LerpToTarget, { target });
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, AnimationDelay, { delay });
    zox_set(e, AnimationLength, { length });
}