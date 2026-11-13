void follow_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_out(TargetPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_o(TargetPosition, target_position);
        if (behaviour->value != zox_behaviour_follow) {
            continue;
        }
        const entity target = zox_get_link(world, e, FollowTarget);
        if (!zox_valid(target)) {
            continue;
        }
        float3 position = zox_getv(target, Position3D);
        target_position->value = position;
    }
    zox_sys_on_end();
} zoxd_system(follow_system);