// A punch (SwingStart, SwingSpeed: skills/sys/melee.c, items) reaches the right arm out where the head looks
//  - its weight rises and falls over the swing (a sine), the arm's raise underneath
//  - the head's facing is +z, a first person camera on it faces -z
//  - only writes the weight while it punches or to end one, a held shoulder (IKHeld, XR hands) is left be
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame
#define ik_punch_reach 8.0f     // past the arm's reach: it straightens

void punch_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingStart);
    zox_sys_in(SwingSpeed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        const entity shoulder = zox_get_link(world, e, ShoulderBoneLink);
        if (!zox_valid(shoulder) || !zox_has(shoulder, IKWeight) || zox_has(shoulder, IKHeld)) {
            continue;
        }
        const double passed = zox_current_time - start->value;
        const byte punching = start->value > 0 && speed->value > 0 && passed >= 0 && passed < speed->value;
        if (!punching) {
            IKWeight* weight = zox_getm(shoulder, IKWeight);
            if (start->value > 0 && weight->value != 0) {
                weight->value = 0;
            }
            continue;
        }
        entity head = zox_get_link(world, e, HeadBoneLink);
        if (!zox_valid(head)) {
            continue;
        }
        float3 facing = float3_forward;
        const entity camera = zox_get_child_by_id(world, head, zox_id(Camera));
        if (zox_valid(camera)) {
            head = camera;
            facing = float3_backward;
        }
        const float3 look = float4_rotate_float3(zox_getv(head, Rotation3D), facing);
        const float3 from = zox_getv(shoulder, Position3D);
        zox_getm(shoulder, IKTarget)->value = float3_add(from, float3_scale(look, ik_punch_reach));
        zox_getm(shoulder, IKWeight)->value = sinf(M_PI * passed / speed->value);
    }
    zox_sys_on_end();
} zoxd_system(punch_ik_system);
