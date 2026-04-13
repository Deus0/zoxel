#define zox_alignment_centre 0
#define zox_alignment_left 1
#define zox_alignment_right 2
#define zox_alignment_top 3
#define zox_alignment_bottom 4
#define zox_alignment_top_right 5
#define zox_alignment_top_left 6
#define zox_alignment_bottom_right 7
#define zox_alignment_bottom_left 8

// Scale for the bounds
float4 alignment_to_scaler(byte alignment) {
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
    }

    return (float4) {
        0.5f,
        0.5f,
        0.5f,
        0.5f
    };
}
