// Heads turn to their LookTarget (a speaker in a dialogue), eased, and back ahead without one (the body's twist off)
//  - placing a block the head turns part way to it (SwingTarget, head_place_nudge); not a first person head: the
//    camera on it already aims there, turning it would move the view
//  - camera_look_at turns -z to a point, so it's given the point mirrored: the face (+z) turns to the target
//  - in its parent's space (the chest), past the neck's reach (behind) it looks ahead
const float head_look_speed = 6;        // a second, of the way left
const float head_look_reach = 0.25f;    // the target's direction, forward of this (z, from 1 ahead to -1 behind)
const float head_place_nudge = 0.6f;    // of the way to a place's block the head turns

// the point a head's skeleton places at, while it's placing
static inline byte head_place_point(ecs* world, entity head, entity skeleton, float3* point) {
    if (!zox_valid(skeleton) || zox_getv(skeleton, SwingKind) != zox_swing_place) {
        return 0;
    }
    const double passed = zox_current_time - zox_getv(skeleton, SwingStart);
    *point = zox_getv(skeleton, SwingTarget);
    return passed >= 0 && passed < zox_getv(skeleton, SwingSpeed) && !float3_equals(*point, float3_zero) &&
        !zox_valid(zox_get_child_by_id(world, head, zox_id(Camera)));
}

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
        // ahead: the body's twist (a punch's) turned back off it, so the look holds (ik/sys/spine.c)
        const entity skeleton = zox_get_link(world, e, SkeletonLink);
        const float twist = zox_valid(skeleton) ? zox_getv(skeleton, SpineTwistHead) : 0;
        const float4 ahead = twist ? quaternion_from_euler((float3) { 0, -twist, 0 }) : quaternion_identity;
        float4 look = ahead;
        float3 point;
        float share = 1;
        byte has_point = zox_valid(target->value);
        if (has_point) {
            point = look_point(world, target->value);
        } else if (head_place_point(world, e, skeleton, &point)) {
            has_point = 1;
            share = head_place_nudge;
        }
        const entity parent = zox_get_parent(world, e);
        if (has_point && zox_valid(parent) && zox_has(parent, Rotation3D)) {
            const float3 mirrored = float3_sub(float3_scale(position->value, 2), point);
            const float4 world_look = camera_look_at(position->value, mirrored);
            const float4 local = quaternion_rotate(quaternion_inverse(zox_getv(parent, Rotation3D)), world_look);
            if (float4_rotate_float3(local, float3_forward).z > head_look_reach) {
                look = quaternion_slerp(ahead, local, share);
            }
        } else if (!has_point && float4_equals(rotation->value, ahead)) {
            continue;
        }
        rotation->value = quaternion_slerp(rotation->value, look, blend);
    }
    zox_sys_on_end();
} zoxd_system(head_look_system);
