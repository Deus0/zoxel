// a first person camera's look up and down (its pitch), as mouse look and a gamepad's stick turn it
//  (controllers3/sys/rotation.c): its local turn (on the head) pitched by delta radians, within limit of level
//  - as euler, the old way (pitch, yaw, roll) round trips; looking far up it can come back rolled (tst guard_pitch
//    checks a camera pitched in steps stays level)
static inline float4 camera_pitch_add(float4 local, float delta, float limit) {
    float3 euler = quaternion_to_euler(local);
    euler.x += delta;
    if (euler.x < -limit) {
        euler.x = -limit;
    } else if (euler.x > limit) {
        euler.x = limit;
    }
    return euler_to_quaternion(euler);
}
