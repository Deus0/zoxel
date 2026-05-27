zox_sys2(ShoulderRaiseSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ShoulderBoneLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ShoulderBoneLink, shoulder);
        if (!zox_valid(shoulder->value)) {
            continue;
        }
        // NOTE: For some reason our models are backward??
        float3 euler = { degrees_to_radians * -115.0f, 0, 0 };
        zox_muter(shoulder->value, LocalRotation3D, rotation);
        rotation->value = euler_to_quaternion(euler);
    }
} zox_sys_end(ShoulderRaiseSystem);
