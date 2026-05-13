const byte max_position_checks = 255;

int3 chunk_position_fix2(float3 real_position, int3 chunk_position) {
    if (real_position.x < 0) chunk_position.x -= 1;
    if (real_position.y < 0) chunk_position.y -= 1;
    if (real_position.z < 0) chunk_position.z -= 1;
    return chunk_position;
}

static inline int positionf_to_positionv1(float positionf, float scale) {
    return (int) floor(positionf / scale);
}

static inline int3 positionf_to_positionv(float3 positionf, float scale) {
    return (int3) {
        positionf_to_positionv1(positionf.x, scale),
        positionf_to_positionv1(positionf.y, scale),
        positionf_to_positionv1(positionf.z, scale)
    };
}

int3 real_position_to_chunk_position(float3 positionf, byte chunk_length, float scale) {
    int3 positionv = positionf_to_positionv(positionf, scale);
    if (positionf.x < 0) positionv.x += 1;
    if (positionf.y < 0) positionv.y += 1;
    if (positionf.z < 0) positionv.z += 1;
    int3 chunk_position = (int3) {
        positionv.x / chunk_length,
        positionv.y / chunk_length,
        positionv.z / chunk_length
    };
    return chunk_position_fix2(positionf, chunk_position);
}

static inline byte3 get_positionl_byte3_2(int3 positionv, byte3 chunk_size, byte3 terrain_chunk_size) {
    byte3 positionl;
    if (positionv.x < 0) positionl.x = chunk_size.x - 1 + ((positionv.x + 1) % terrain_chunk_size.x);
    else positionl.x = positionv.x % terrain_chunk_size.x;
    if (positionv.y < 0) positionl.y = chunk_size.y - 1 + ((positionv.y + 1) % terrain_chunk_size.y);
    else positionl.y = positionv.y % terrain_chunk_size.y;
    if (positionv.z < 0) positionl.z = chunk_size.z - 1 + ((positionv.z + 1) % terrain_chunk_size.z);
    else positionl.z = positionv.z % terrain_chunk_size.z;
    return positionl;
}

static inline int3 voxel_chunk_position_xz(int3 chunk_position, int3 chunk_size) {
    return int3_multiply_int3(chunk_position, chunk_size);
}

static inline int3 get_chunk_positionv(int3 chunk_position, int3 chunk_size) {
    int3 positionv = int3_multiply_int3(chunk_position, chunk_size);
    return positionv;
}

float3 voxel_to_real_position(int3 positionv, float terrain_voxel_scale, float chunk_voxel_scale) {
    float3 positionf = int3_to_float3(positionv);
    float3_scale_p(&positionf, terrain_voxel_scale);
    // get middle of voxel position
    return float3_add(positionf, float3_scale(float3_halff, chunk_voxel_scale));
}

// NOTE: this doesn't account for local chunk depth difference to terrain grid
float3 local_to_real_position_character(byte3 in_chunk_position, int3 chunk_grid_position, float3 bounds, float scale) {
    // const float scale = get_terrain_voxel_scale(depth) * vox_scale;
    int3 grid_position = int3_add(chunk_grid_position, byte3_to_int3(in_chunk_position));
    float3 position = int3_to_float3(grid_position);
    // zox_log("scale: %f", scale)
    float3_scale_p(&position, scale);
    position.x += scale / 2.0f;
    position.z += scale / 2.0f;
    position.y += bounds.y / 2.0f;
    position.y += 0.05f; // extra
    return position;
}

static inline float positionv_to_positionf1(int positionv, float scale) {
    return positionv * scale;
}

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
        int_div(pos.x, (int)terrain_chunk_size.x),
        int_div(pos.y, (int)terrain_chunk_size.y),
        int_div(pos.z, (int)terrain_chunk_size.z)
    };

    // Adjust for negatives so that they map to the correct chunk
    if (positionv.x < 0) chunk_position.x -= 1;
    if (positionv.y < 0) chunk_position.y -= 1;
    if (positionv.z < 0) chunk_position.z -= 1;

    return chunk_position;
}
