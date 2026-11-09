entity spawn_lerp_float(ecs* world, entity target, entity id, float from, float to, double length) {
    entity e = zox_new();
    zox_set_unique_name(e, "lerp_float");
    zox_setv(e, AnimationTarget, target);
    zox_setv(e, AnimationTargetComponent, id);
    zox_setv(e, AnimationStartTime, zox_current_time);
    zox_setv(e, AnimationDuration, length);
    zox_setv(e, LerpFloatFrom, from);
    zox_setv(e, LerpFloatTo, to);
    return e;
}

entity spawn_lerp_float_old(ecs* world, entity target, entity id, float to, double length) {
    float from = zox_get_idv(target, id, float);
    return spawn_lerp_float(world, target, id, from, to, length);
}

// shakes target's LocalPosition3D, refreshes the shake if one is already active
entity spawn_shake_position(ecs* world, entity target, float strength, double length) {
    if (!zox_valid(target) || !zox_has(target, LocalPosition3D)) {
        return 0;
    }
    entity existing = zox_get_link(world, target, ShakeLink);
    if (zox_valid(existing) && zox_has(existing, ShakePosition)) {
        zox_setv(existing, AnimationStartTime, zox_current_time);
        zox_setv(existing, AnimationDuration, length);
        zox_setv(existing, ShakeStrength, strength);
        return existing;
    }
    entity e = zox_new();
    zox_set_unique_name(e, "shake_position");
    zox_add(e, ShakePosition);
    zox_setv(e, AnimationTarget, target);
    zox_setv(e, AnimationStartTime, zox_current_time);
    zox_setv(e, AnimationDuration, length);
    zox_setv(e, ShakeStrength, strength);
    zox_setv(e, ShakeOrigin, zox_getv(target, LocalPosition3D));
    zox_setv(e, ShakeOffset, float3_zero);
    // NOTE: flecs removes this pair when the shake is deleted
    zox_link(world, target, ShakeLink, e);
    return e;
}
