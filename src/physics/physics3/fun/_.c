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

// up is against gravity, any direction (+y without any)
static inline float3 physics3_up(const float3 gravity) {
    const float length = float3_length(gravity);
    if (length <= 0) {
        return (float3) { 0, 1, 0 };
    }
    return float3_scale(gravity, -1 / length);
}

// the axis nearest up (0 x, 1 y, 2 z) and its sign along it: boxes against voxels have a floor on an axis
//  (gravity off any axis uses its biggest part), +y without any gravity
static inline byte physics3_up_axis(const float3 gravity, int* sign) {
    const float3 g = { float_abs(gravity.x), float_abs(gravity.y), float_abs(gravity.z) };
    if (g.x == 0 && g.y == 0 && g.z == 0) {
        *sign = 1;
        return 1;
    }
    const byte axis = g.x > g.y && g.x >= g.z ? 0 : (g.y >= g.z ? 1 : 2);
    const float along = axis == 0 ? gravity.x : (axis == 1 ? gravity.y : gravity.z);
    *sign = along > 0 ? -1 : 1;
    return axis;
}

// a box's half extent along a direction (its radius rolling on a ground under that up)
static inline float physics3_half_along(const float3 half_extents, const float3 direction) {
    return float_abs(direction.x) * half_extents.x + float_abs(direction.y) * half_extents.y + float_abs(direction.z) * half_extents.z;
}

// spin that rolls a body of this radius over the ground (under up) without slipping:
// the point touching the ground stays still, w = up x v / radius, in world space
static inline float3 physics3_rolling_omega(const float3 velocity, const float3 up, const float radius) {
    if (radius <= 0) {
        return float3_zero;
    }
    return float3_scale(float3_cross(up, velocity), 1 / radius);
}
