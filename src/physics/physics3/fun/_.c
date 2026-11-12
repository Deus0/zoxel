// Omega3D and Alpha3D are in the body's frame (omega3_system: rotation = rotation * delta),
// these turn a world space spin into the body's frame and back. Unit quaternions only.
static inline float3 physics3_world_to_body(const float4 rotation, const float3 world) {
    return float4_rotate_float3(float4_inverse(rotation), world);
}

static inline float3 physics3_body_to_world(const float4 rotation, const float3 body) {
    return float4_rotate_float3(rotation, body);
}

// adds a world space angular acceleration to a body (i.e. a hit, a spell pushing it around)
static inline void physics3_add_alpha_world(const float4 rotation, float3* alpha, const float3 world_alpha) {
    *alpha = float3_add(*alpha, physics3_world_to_body(rotation, world_alpha));
}

// spin that rolls a body of this radius over the ground (up = +y) without slipping:
// the point touching the ground stays still, w = up x v / radius, in world space
static inline float3 physics3_rolling_omega(const float3 velocity, const float radius) {
    if (radius <= 0) {
        return float3_zero;
    }
    return (float3) { velocity.z / radius, 0, -velocity.x / radius };
}
