// Convert terrain-scale voxel coords (positionv) into terrain chunk coords (positionc)
// using terrain_chunk_size (NOT local chunk size).
static inline int3 positionv_to_positionc(
    const int3 positionv,         // voxel coords in terrain scale
    const byte3 terrain_chunk_size// voxel dimensions of a terrain chunk
) {
    int3 pos = positionv;

    // Offset negatives before division so integer truncation works properly
    if (pos.x < 0) pos.x += 1;
    if (pos.y < 0) pos.y += 1;
    if (pos.z < 0) pos.z += 1;

    // Base chunk grid coords (terrain-scale)
    int3 chunk_position = (int3) {
        pos.x / (int)terrain_chunk_size.x,
        pos.y / (int)terrain_chunk_size.y,
        pos.z / (int)terrain_chunk_size.z
    };

    // Adjust for negatives so that they map to the correct chunk
    if (positionv.x < 0) chunk_position.x -= 1;
    if (positionv.y < 0) chunk_position.y -= 1;
    if (positionv.z < 0) chunk_position.z -= 1;

    return chunk_position;
}
