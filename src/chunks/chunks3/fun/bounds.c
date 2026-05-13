float3 calculate_vox_bounds(int3 size, float scale) {
    float3 b = float3_from_int3(size);
    float3_scale_p(&b, scale * 0.5f);
    return b;
}
