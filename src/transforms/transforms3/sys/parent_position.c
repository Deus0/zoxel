void set_position_from_parents(ecs *world, entity parent, float3 *position, float3 local) {
    if (!parent || !zox_has(parent, Position3D) || !zox_has(parent, Rotation3D)) {
        return;
    }

    float3 parent_position = zox_get_value(parent, Position3D);
    float4 parent_rotation = zox_get_value(parent, Rotation3D);

    *position = local;
    float4_rotate_float3_p(parent_rotation, position);
    float3_add_float3_p(position, parent_position);
}

zox_sys2(ParentPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_in(LocalPosition3D);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parent);
        zox_sys_i(LocalPosition3D, local);
        zox_sys_o(Position3D, position);

        if (!zox_valid(parent->value)) {
            continue;
        }

        set_position_from_parents(world, parent->value, &position->value, local->value);
    }
} zox_sys_end(ParentPositionSystem);
