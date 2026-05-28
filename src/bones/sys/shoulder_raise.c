// NOTE: Simply raises arm up
zox_sys2(ShoulderRaiseSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaiseShoulder);
    zox_sys_in(ShoulderBoneLink);
    zox_sys_in(HeadBoneLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaiseShoulder, state);
        zox_sys_i(ShoulderBoneLink, shoulder);
        zox_sys_i(HeadBoneLink, head);
        if (!zox_valid(shoulder->value)) {
            continue;
        }
        zox_muter(shoulder->value, LocalRotation3D, rotation);
        if (!state->value) {
            rotation->value = quaternion_identity;  // set to original
            continue;
        }
        entity head_bone = head->value;
        if (!zox_valid(head_bone)) {
            continue;
        }
        head_bone = zox_get_child_by_id(world, head->value, zox_id(Camera));
        if (!zox_valid(head_bone)) {
            continue;
        }
        float4 head_rotation = zox_getv(head_bone, LocalRotation3D);
        float3 euler = quaternion_to_euler(head_rotation);
        euler.x = -euler.x;
        euler.x -= (115 * degrees_to_radians);
        euler.y = 0;
        euler.z = 0;
        rotation->value = euler_to_quaternion(euler);
        // float3 test_euler = float3_mulf((float3)  { -115.0f, 0, 0 }, degrees_to_radians);
        // rotation->value = euler_to_quaternion(test_euler);
        // zox_log("Compare Euler: Real: [%f] Target: [%f]", euler.x * radians_to_degrees, test_euler.x * radians_to_degrees);
    }
} zox_sys_end(ShoulderRaiseSystem);
