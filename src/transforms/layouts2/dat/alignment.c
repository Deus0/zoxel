#define zox_alignment_centre 0
#define zox_alignment_left 1
#define zox_alignment_right 2
#define zox_alignment_top 3
#define zox_alignment_bottom 4
#define zox_alignment_top_right 5
#define zox_alignment_top_left 6
#define zox_alignment_bottom_right 7
#define zox_alignment_bottom_left 8

static inline float2 alignment_to_anchor(byte alignment) {
    if (alignment == zox_alignment_left) {
        return (float2) { 0, 0.5f };
    } else if (alignment == zox_alignment_right) {
        return (float2) { 1, 0.5f };
    } else if (alignment == zox_alignment_centre) {
        return float2_half;
    } else {
        zox_logw("Alignment not added to anchor table [%i]", alignment);
        return float2_half;
    }
}

// Scale for the bounds
static inline float4 alignment_to_scaler(byte alignment) {
    if (alignment == zox_alignment_left) {
        return (float4) {
            0,
            1,
            0.5f,
            0.5f
        };
    } else if (alignment == zox_alignment_right) {
        return (float4) {
            1,
            0,
            0.5f,
            0.5f
        };
    } else if (alignment == zox_alignment_bottom_right) {
        return (float4) {
            1, 0,
            0, 1
        };
    }
    return (float4) { 0.5f, 0.5f, 0.5f, 0.5f };
}
