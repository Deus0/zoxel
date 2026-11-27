// a player's stick (or WASD) as a move in its character's own frame, each axis -1 to 1: x its side, z forward;
//  characters face +z (is_camera_positive_z), so forward is +z then
static inline float3 player3_move_local(float2 stick) {
    float3 movement = { stick.x, 0, -stick.y };
    if (is_camera_positive_z) {
        movement.x *= -1;
        movement.z *= -1;
    }
    return movement;
}

// third person (the orbit camera): the rotation its character faces and walks by, the camera's facing, flat
//  - the camera looks along -z (camera_look_at) and is pitched down: its yaw from its forward, not its euler (a pitched
//    euler can come back yawed half round, which walked W towards the camera)
static inline float4 third_person_move_rotation(float4 camera_rotation) {
    const float3 forward = float4_rotate_float3(camera_rotation, (float3) { 0, 0, -1 });
    const float yaw = is_camera_positive_z ? atan2f(forward.x, forward.z) : atan2f(-forward.x, -forward.z);
    return quaternion_from_axis_angle(float3_up, yaw);
}
