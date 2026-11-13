// Heads turn to their LookTarget (a speaker in a dialogue), eased, and back ahead without one
//  - camera_look_at turns -z to a point, so it's given the point mirrored: the face (+z) turns to the target
//  - in its parent's space (the chest), past the neck's reach (behind) it looks ahead
const float head_look_speed = 6;        // a second, of the way left
const float head_look_reach = 0.25f;    // the target's direction, forward of this (z, from 1 ahead to -1 behind)

void head_look_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float blend = float_min(1, zox_delta_time * head_look_speed);
    zox_sys_begin();
    zox_sys_in(LookTarget);
    zox_sys_in(Position3D);
    zox_sys_out(LocalRotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LookTarget, target);
        zox_sys_i(Position3D, position);
        zox_sys_o(LocalRotation3D, rotation);
        float4 look = quaternion_identity;
        const entity parent = zox_get_parent(world, e);
        if (zox_valid(target->value) && zox_valid(parent) && zox_has(parent, Rotation3D)) {
            const float3 point = look_point(world, target->value);
            const float3 mirrored = float3_sub(float3_scale(position->value, 2), point);
            const float4 world_look = camera_look_at(position->value, mirrored);
            const float4 local = quaternion_rotate(quaternion_inverse(zox_getv(parent, Rotation3D)), world_look);
            if (float4_rotate_float3(local, float3_forward).z > head_look_reach) {
                look = local;
            }
        } else if (float4_equals(rotation->value, quaternion_identity)) {
            continue;
        }
        rotation->value = quaternion_slerp(rotation->value, look, blend);
    }
    zox_sys_on_end();
} zoxd_system(head_look_system);
