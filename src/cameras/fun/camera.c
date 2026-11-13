// the rotation for a camera at position to look at target, y up
//  - cameras look down -z (opengl view space), so the identity rotation looks at -z
//  - yaw about y then pitch about x, no roll; looking straight up or down keeps the yaw at 0
float4 camera_look_at(
    float3 position,
    float3 target)
{
    float3 direction = float3_subtract(target, position);
    const float length = float3_length(direction);
    if (length <= 0.000001f) {
        return quaternion_identity;
    }
    direction = float3_scale(direction, 1 / length);
    const float flat = sqrtf(direction.x * direction.x + direction.z * direction.z);
    const float yaw = flat > 0.000001f ? atan2f(-direction.x, -direction.z) : 0;
    const float pitch = atan2f(direction.y, flat);
    return quaternion_multiply(
        quaternion_from_axis_angle(float3_up, yaw),
        quaternion_from_axis_angle(float3_right, pitch));
}

// where a world point lands in a camera's view, from its ViewProjectionMatrix
//  - x and y: -1 to 1 across the viewport (y up), z: depth, w > 0 when in front of the camera
float4 camera_project_point(
    const float4x4 view_projection,
    float3 point)
{
    float4 clip = float4x4_multiply_float4(view_projection, (float4) { point.x, point.y, point.z, 1 });
    if (clip.w != 0) {
        const float w = clip.w;
        clip.x /= w;
        clip.y /= w;
        clip.z /= w;
    }
    return clip;
}
