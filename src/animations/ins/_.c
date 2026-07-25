
entity spawn_lerp_float(ecs* world, entity target, entity id, float to, double length) {
    float from = zox_get_idv(target, id, float);
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
