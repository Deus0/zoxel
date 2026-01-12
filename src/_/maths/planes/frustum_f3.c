float3 generate_plane_normal_f3(const float3 a, const float3 b, const float3 c) {
    const float3 vab = float3_sub(a, b);
    const float3 vac = float3_sub(a, c);
    return float3_normalize(float3_cross(vab, vac));
}

plane calculate_plane_from_points_f3(const float3 a, const float3 b, const float3 c) {
    plane p;
    p.normal = generate_plane_normal_f3(a, b, c);
    p.distance = float3_dot(p.normal, a);
    return p;
}