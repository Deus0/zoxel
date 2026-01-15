// returns simple camera distance for chunks
static inline byte get_camera_chunk_distance(int3 camera_position, int3 chunk_position) {
    return (byte) int3_max(chunk_position, camera_position);
}

static inline byte get_camera_chunk_distance_xz(int3 camera_position, int3 chunk_position) {
    return (byte) int3_max_xz((int3) { chunk_position.x, 0, chunk_position.z }, camera_position);
}


static inline int int2_max2(int2 a, int2 b) {
    return int_max(int_abs(b.x - a.x), int_abs(b.y - a.y));
}

static inline byte get_camera_chunk2_distance(int2 a, int2 b) {
    return (byte) int2_max2(b, a);
}