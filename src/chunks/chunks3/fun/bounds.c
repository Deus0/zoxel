float3 calculate_vox_bounds(int3 size, float scale) {
    float3 b = float3_from_int3(size);
    float3_scale_p(&b, scale * 0.5f);
    return b;
}

// Size of a vox when sampled at a lower depth than its node depth (rounds up)
int3 get_vox_size_at_depth(int3 size, byte node_depth, byte depth) {
    if (depth >= node_depth) {
        return size;
    }
    byte shift = node_depth - depth;
    int divisor = 1 << shift;
    return (int3) {
        (size.x + divisor - 1) / divisor,
        (size.y + divisor - 1) / divisor,
        (size.z + divisor - 1) / divisor
    };
}
