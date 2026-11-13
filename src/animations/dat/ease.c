// Tween curves (AnimationEase), t from 0 to 1
#define zox_ease_linear 0
#define zox_ease_out 1          // fast then settles
#define zox_ease_out_back 2     // overshoots a little then settles, a pop in
#define zox_ease_in_back 3      // pulls back a little then goes, a pop out

static inline float zox_ease(byte ease, float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;
    const float u = t - 1;
    switch (ease) {
        case zox_ease_out: return 1 + u * u * u;
        case zox_ease_out_back: return 1 + c3 * u * u * u + c1 * u * u;
        case zox_ease_in_back: return c3 * t * t * t - c1 * t * t;
        default: return t;
    }
}
