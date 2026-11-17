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
