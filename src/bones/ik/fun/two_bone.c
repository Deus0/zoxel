// Two bone ik, analytic: a root, an upper bone (length a) and a lower bone (length b) reaching for a target
//  - the elbow bends towards the pole, out of the line from the root to the target
//  - out of reach the arm straightens towards it, too close it folds as far as it can
//  - writes where the elbow and the end land
static inline void ik_two_bone(float3 root, float a, float b, float3 target, float3 pole, float3* elbow, float3* end) {
    float3 to_target = float3_sub(target, root);
    float d = float3_length(to_target);
    const float3 dir = d > 0.0001f ? float3_scale(to_target, 1.0f / d) : float3_down;
    const float reach_min = fabsf(a - b) + 0.0001f;
    const float reach_max = (a + b) * 0.9999f;
    if (d < reach_min) {
        d = reach_min;
    }
    if (d > reach_max) {
        d = reach_max;
    }
    // law of cosines: the angle at the root, between the target and the elbow
    float cos_root = (a * a + d * d - b * b) / (2 * a * d);
    cos_root = cos_root < -1 ? -1 : (cos_root > 1 ? 1 : cos_root);
    const float sin_root = sqrtf(1 - cos_root * cos_root);
    // the bend's side: the pole, without its part along the line
    float3 bend = float3_sub(pole, float3_scale(dir, float3_dot(pole, dir)));
    if (float3_length(bend) < 0.0001f) {
        bend = float3_cross(dir, float3_right);
        if (float3_length(bend) < 0.0001f) {
            bend = float3_cross(dir, float3_up);
        }
    }
    bend = float3_normalize(bend);
    *elbow = float3_add(root, float3_add(float3_scale(dir, a * cos_root), float3_scale(bend, a * sin_root)));
    *end = float3_add(root, float3_scale(dir, d));
}

// q's twist about axis (unit): its turn about the axis alone (swing twist split), identity if it has none
static inline float4 quaternion_twist(float4 q, float3 axis) {
    const float d = q.x * axis.x + q.y * axis.y + q.z * axis.z;
    float4 twist = { axis.x * d, axis.y * d, axis.z * d, q.w };
    const float length = sqrtf(twist.x * twist.x + twist.y * twist.y + twist.z * twist.z + twist.w * twist.w);
    if (length < 0.00001f) {
        return quaternion_identity;
    }
    return (float4) { twist.x / length, twist.y / length, twist.z / length, twist.w / length };
}

// v without its part along axis (unit)
static inline float3 ik_perpendicular(float3 v, float3 axis) {
    return float3_sub(v, float3_scale(axis, float3_dot(v, axis)));
}

// a bone's turn from its rest line to a line, and its rest side to a side (about the line): both fixed, so the turn
//  doesn't flip about the line as the shortest one (from to) does, half a turn round from its rest (an arm up)
static inline float4 ik_turn_to(float3 rest_line, float3 rest_side, float3 line, float3 side) {
    const float4 q = quaternion_from_to(rest_line, line);
    const float3 axis = float3_normalize(line);
    const float3 turned = ik_perpendicular(float4_rotate_float3(q, rest_side), axis);
    const float3 wanted = ik_perpendicular(side, axis);
    if (float3_length(turned) < 0.0001f || float3_length(wanted) < 0.0001f) {
        return q;
    }
    const float angle = atan2f(float3_dot(float3_cross(turned, wanted), axis), float3_dot(turned, wanted));
    return quaternion_rotate(quaternion_from_axis_angle(axis, angle), q);
}
