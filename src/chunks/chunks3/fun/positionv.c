static inline int3 positionf_to_positionv(float3 positionf, float scale) {
    return (int3) {
        positionf_to_positionv1(positionf.x, scale),
        positionf_to_positionv1(positionf.y, scale),
        positionf_to_positionv1(positionf.z, scale)
    };
}

static inline float3 positionv_to_positionf(int3 positionv, float scale) {
    return (float3) {
        positionv_to_positionf1(positionv.x, scale),
        positionv_to_positionf1(positionv.y, scale),
        positionv_to_positionf1(positionv.z, scale)
    };
}

int3 positionv_to_chunk_position(int3 positionv, int3 chunk_size) {
    int3 positionv2 = positionv;
    if (positionv.x < 0) positionv2.x += 1;
    if (positionv.y < 0) positionv2.y += 1;
    if (positionv.z < 0) positionv2.z += 1;
    int3 chunk_position = int3_div(positionv2, chunk_size);
    // (int3) { positionv.x / chunk_size.x, positionv.y / chunk_size.y, positionv.z / chunk_size.z };
    // because for example -10 / 16 is 0 as an integer, but  coordinates we need a negative chunk position
    if (positionv.x < 0) chunk_position.x -= 1;
    if (positionv.y < 0) chunk_position.y -= 1;
    if (positionv.z < 0) chunk_position.z -= 1;
    return chunk_position;
    // return (int3) { positionv.x / chunk_size.x, positionv.y / chunk_size.y, positionv.z / chunk_size.z };
}

// Convert voxel-space coords (positionv) to local-in-chunk coords (positionl),
// scaling from this chunk's voxel size (chunk_size) to the terrain's local basis (terrain_chunk_size).
// Handles negative chunk positions with tail-window semantics (matches get_positionl_byte3_2).
static inline byte3 positionv_to_positionl(
    const int3  positionv,        // world voxel-space coords
    const int3  chunk_position,   // chunk grid coordinate
    const int3  chunk_size,       // voxel size of *this* chunk
    const byte3 terrain_chunk_size// terrain basis to scale to
) {
    // Origin voxel coord of this chunk
    const int3 base = int3_multiply_int3(chunk_position, chunk_size);

    // Offset inside the chunk in current chunk_size basis
    int3 local = int3_sub(positionv, base);

    // Wrap inside chunk bounds (0..chunk_size-1)
    // This is needed if you get weird offsets from rounding or neighbor fetches.
    local.x = (local.x % chunk_size.x + chunk_size.x) % chunk_size.x;
    local.y = (local.y % chunk_size.y + chunk_size.y) % chunk_size.y;
    local.z = (local.z % chunk_size.z + chunk_size.z) % chunk_size.z;

    // Safeguard to avoid div-by-zero
    const int tx = terrain_chunk_size.x ? terrain_chunk_size.x : 1;
    const int ty = terrain_chunk_size.y ? terrain_chunk_size.y : 1;
    const int tz = terrain_chunk_size.z ? terrain_chunk_size.z : 1;

    byte3 positionl;

    // ----- X -----
    if (chunk_position.x >= 0) {
        // Scale from chunk_size → terrain_chunk_size
        positionl.x = (local.x * tx) / chunk_size.x;
    } else {
        // Tail-window logic: measure distance from end
        int dist_from_end = (chunk_size.x - 1) - local.x;
        positionl.x = (tx - 1) - ((dist_from_end * tx) / chunk_size.x);
        // Shift into tail window
        positionl.x = (chunk_size.x - tx) + (positionl.x % tx);
    }

    // ----- Y -----
    if (chunk_position.y >= 0) {
        positionl.y = (local.y * ty) / chunk_size.y;
    } else {
        int dist_from_end = (chunk_size.y - 1) - local.y;
        positionl.y = (ty - 1) - ((dist_from_end * ty) / chunk_size.y);
        positionl.y = (chunk_size.y - ty) + (positionl.y % ty);
    }

    // ----- Z -----
    if (chunk_position.z >= 0) {
        positionl.z = (local.z * tz) / chunk_size.z;
    } else {
        int dist_from_end = (chunk_size.z - 1) - local.z;
        positionl.z = (tz - 1) - ((dist_from_end * tz) / chunk_size.z);
        positionl.z = (chunk_size.z - tz) + (positionl.z % tz);
    }

    return positionl;
}
