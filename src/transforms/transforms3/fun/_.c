void set_rotation_from_parents(ecs *world, entity parent, float4 *rotation3D, float4 local_rotation3D) {
    if (!parent || !zox_has(parent, Rotation3D)) {
        return;
    }
    float4 parent_rotation = zox_getv(parent, Rotation3D);
    *rotation3D = parent_rotation;
    quaternion_rotate_quaternion_p(rotation3D, local_rotation3D);
}

void set_position_from_parents(ecs *world, entity parent, float3 *position, float3 local) {
    if (!parent || !zox_has(parent, Position3D) || !zox_has(parent, Rotation3D)) {
        return;
    }
    float3 parent_position = zox_getv(parent, Position3D);
    float4 parent_rotation = zox_getv(parent, Rotation3D);
    *position = local;
    float4_rotate_float3_p(parent_rotation, position);
    float3_add_float3_p(position, parent_position);
}
