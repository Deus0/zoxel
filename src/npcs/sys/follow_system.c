void FollowSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(DisableMovement);
    zox_sys_in(FollowTarget);
    zox_sys_out(TargetPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(DisableMovement, disable);
        zox_sys_i(FollowTarget, target_entity);
        zox_sys_o(TargetPosition, target_position);

        if (disable->value || behaviour->value != zox_behaviour_follow) {
            continue;
        }

        if (!zox_valid(target_entity->value)) {
            continue;
        }

        zox_geter_value(target_entity->value, Position3D, float3, position);
        target_position->value = position;
    }
} zoxd_system2(FollowSystem);