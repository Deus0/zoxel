static inline int positionf_to_positionv1(float positionf, float scale) {
    return (int) floor(positionf / scale);
}

static inline float positionv_to_positionf1(int positionv, float scale) {
    return positionv * scale;
}
/*int3 get_positionl(
    int3 positionv, // Terrain Global Position
    int3 s     // Terrain Chunk Size - Max Depth
) {
    positionv.x %= s.x;
    positionv.y %= s.y;
    positionv.z %= s.z;
    if (positionv.x < 0) positionv.x = s.x - 1 + positionv.x;
    if (positionv.y < 0) positionv.y = s.y - 1 + positionv.y;
    if (positionv.z < 0) positionv.z = s.z - 1 + positionv.z;
    return positionv;
}*/

// Obsolete, this doesnt handle local depth differences
static inline byte3 get_positionl_byte3(int3 positionv, byte3 chunk_size) {
    byte3 positionl;
    if (positionv.x < 0) {
        positionl.x = chunk_size.x - 1 + ((positionv.x + 1) % chunk_size.x);
    } else {
        positionl.x = positionv.x % chunk_size.x;
    }
    if (positionv.y < 0) {
        positionl.y = chunk_size.y - 1 + ((positionv.y + 1) % chunk_size.y);
    } else {
        positionl.y = positionv.y % chunk_size.y;
    }
    if (positionv.z < 0) {
        positionl.z = chunk_size.z - 1 + ((positionv.z + 1) % chunk_size.z);
    } else {
        positionl.z = positionv.z % chunk_size.z;
    }
    return positionl;
}

// Map local-in-chunk coords (positionl) to world-voxel coords (positionv),
// scaling from the terrain's local basis (terrain_chunk_size) to this chunk's voxel size (chunk_size).
// Handles negative chunks the same way your get_positionl_* does: negative axes anchor to the
// high end of the chunk window.
//
// Assumes: 0 < terrain_chunk_size.{x,y,z} <= chunk_size.{x,y,z}
static inline int3 positionl_to_positionv(
    const byte3 positionl,        // local coords (could be in terrain basis or tail-window form)
    const int3  chunk_position,   // chunk grid coordinate (sign matters)
    const int3  chunk_size,       // voxel size of *this* chunk (may vary by depth)
    const byte3 terrain_chunk_size// terrain basis used to compute positionl
) {
    // World-space origin (voxel coords) of this chunk.
    // If your X/Z sizes vary too, pass a precalculated origin instead of this multiply.
    const int3 base = int3_multiply_int3(chunk_position, chunk_size);

    // Safeguard (avoid div-by-zero). Terrain sizes should never be 0.
    const int tx = terrain_chunk_size.x ? terrain_chunk_size.x : 1;
    const int ty = terrain_chunk_size.y ? terrain_chunk_size.y : 1;
    const int tz = terrain_chunk_size.z ? terrain_chunk_size.z : 1;

    int3 local; // scaled local (0..chunk_size-1 per axis)

    // ----- X -----
    {
        const int u  = (int)positionl.x; // incoming local
        const int cs = chunk_size.x;
        int un; // normalized 0..tx-1, measured from the correct edge
        if (chunk_position.x >= 0) {
            // front-window semantics
            un = u % tx;
            local.x = (un * cs) / tx;
        } else {
            // tail-window semantics
            if (u >= cs - tx && u < cs) {
                // already in tail-window [cs-tx .. cs-1]; convert to distance from the end
                un = (cs - 1) - u;
            } else {
                // normalize a front-window value into "distance from end"
                un = (tx - 1) - (u % tx);
            }
            local.x = cs - 1 - (un * cs) / tx;
        }
    }

    // ----- Y -----
    {
        const int u  = (int)positionl.y;
        const int cs = chunk_size.y;
        int un;
        if (chunk_position.y >= 0) {
            un = u % ty;
            local.y = (un * cs) / ty;
        } else {
            if (u >= cs - ty && u < cs) un = (cs - 1) - u;
            else                         un = (ty - 1) - (u % ty);
            local.y = cs - 1 - (un * cs) / ty;
        }
    }

    // ----- Z -----
    {
        const int u  = (int)positionl.z;
        const int cs = chunk_size.z;
        int un;
        if (chunk_position.z >= 0) {
            un = u % tz;
            local.z = (un * cs) / tz;
        } else {
            if (u >= cs - tz && u < cs) un = (cs - 1) - u;
            else                         un = (tz - 1) - (u % tz);
            local.z = cs - 1 - (un * cs) / tz;
        }
    }

    return int3_add(base, local);
}
