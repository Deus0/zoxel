// clockwise or counter clockwise determines normal direction
double3 generate_plane_normal_d3(const double3 a, const double3 b, const double3 c) {
    const double3 ab = double3_sub(a, b);
    const double3 ac = double3_sub(a, c);
    return double3_normalize(double3_cross(ab, ac));
}

plane calculate_plane_from_points_d3(const double3 a, const double3 b, const double3 c) {
    plane result_plane;
    result_plane.normal = double3_to_float3(generate_plane_normal_d3(a, b, c));
    result_plane.distance = double3_dot(float3_to_double3(result_plane.normal), a);
    return result_plane;
}

// Original
void calculate_planes_from_frustum_d3_outwards(const double3 *frustum, plane *planes) {
    // Left is (-1,-1,-1) -> (-1,+1,-1) -> (-1,-1,+1) => counter clockwise
    planes[0] = calculate_plane_from_points_d3(frustum[0], frustum[3], frustum[4]); // 0, 3, 4, 7 L
    // Right is (+1,-1,+1) ->  (+1,+1,-1) -> (+1,-1,-1) => clockwise
    planes[1] = calculate_plane_from_points_d3(frustum[5], frustum[2], frustum[1]); // 1, 2, 5, 6 R
    // Down is (-1,-1,-1) -> (+1,-1,-1) -> (-1,-1,+1) => counter clockwise
    planes[2] = calculate_plane_from_points_d3(frustum[4], frustum[1], frustum[0]); // 0, 1, 4, 5 B
    // Up is (-1,+1,+1) -> (+1,+1,+1) -> (-1,+1,-1) => clockwise
    planes[3] = calculate_plane_from_points_d3(frustum[3], frustum[6], frustum[7]); // 2, 3, 6, 7 T
    // Near is 1 (+1,-1,-1) -> 2 (+1,+1,-1) -> 3 (-1,+1,-1) => clockwise
    planes[4] = calculate_plane_from_points_d3(frustum[1], frustum[2], frustum[3]); // 0, 1, 2, 3 N
    // Far is 1 (+1,-1,+1) -> 2 (+1,+1,+1) -> 3 (-1,+1,+1) => counter clockwise
    planes[5] = calculate_plane_from_points_d3(frustum[7], frustum[6], frustum[5]); // 4, 5, 6, 7 F
}