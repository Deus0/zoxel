const byte max_position_checks = 255;

static inline int real_position_to_block_position1(float positionf, float scale) {
    return (int) floor(positionf / scale);
}

// NOTE: For Negative real, we move chunk position away from 0
int chunk_position_fix1(float real_position, int chunk_position) {
    return real_position >= 0 ? chunk_position : chunk_position - 1;
}

int real_position_to_chunk_position1(float positionf, byte chunk_length, float scale) {
    int block_position = real_position_to_block_position1(positionf, scale);
    if (positionf < 0) block_position += 1;
    int chunk_position = block_position / chunk_length;
    return chunk_position_fix1(positionf, chunk_position);
}

int3 chunk_position_fix2(float3 real_position, int3 chunk_position) {
    if (real_position.x < 0) chunk_position.x -= 1;
    if (real_position.y < 0) chunk_position.y -= 1;
    if (real_position.z < 0) chunk_position.z -= 1;
    return chunk_position;
}

static inline int3 real_position_to_block_position(float3 positionf, float scale) {
    return (int3) {
        real_position_to_block_position1(positionf.x, scale),
        real_position_to_block_position1(positionf.y, scale),
        real_position_to_block_position1(positionf.z, scale)
    };
}

int3 real_position_to_chunk_position(float3 positionf, byte chunk_length, float scale) {
    int3 block_position = real_position_to_block_position(positionf, scale);
    if (positionf.x < 0) block_position.x += 1;
    if (positionf.y < 0) block_position.y += 1;
    if (positionf.z < 0) block_position.z += 1;
    int3 chunk_position = (int3) {
        block_position.x / chunk_length,
        block_position.y / chunk_length,
        block_position.z / chunk_length
    };
    return chunk_position_fix2(positionf, chunk_position);
}

static inline byte3 block_position_to_local_position(int3 block_position, byte terrain_depth, byte chunk_depth) {
    // int3 chunk_size = int3_single(powers_of_two[chunk_depth]);
    int3 terrain_chunk_size = int3_single(powers_of_two[terrain_depth]);
    byte3 positionl;
    if (block_position.x < 0) {
        positionl.x = terrain_chunk_size.x - 1 + ((block_position.x + 1) % terrain_chunk_size.x);
    } else {
        positionl.x = block_position.x % terrain_chunk_size.x;
    }
    if (block_position.y < 0) {
        positionl.y = terrain_chunk_size.y - 1 + ((block_position.y + 1) % terrain_chunk_size.y);
    } else {
        positionl.y = block_position.y % terrain_chunk_size.y;
    }
    if (block_position.z < 0) {
        positionl.z = terrain_chunk_size.z - 1 + ((block_position.z + 1) % terrain_chunk_size.z);
    } else {
        positionl.z = block_position.z % terrain_chunk_size.z;
    }
    // NOTE: THis divides to account for differences of depth!
    for (int i = chunk_depth; i < terrain_depth; i++) {
        positionl.x /= 2;
        positionl.y /= 2;
        positionl.z /= 2;
    }
    return positionl;
}

static inline byte3 get_positionl_byte3_2(int3 block_position, byte3 chunk_size, byte3 terrain_chunk_size) {
    byte3 positionl;
    if (block_position.x < 0) positionl.x = chunk_size.x - 1 + ((block_position.x + 1) % terrain_chunk_size.x);
    else positionl.x = block_position.x % terrain_chunk_size.x;
    if (block_position.y < 0) positionl.y = chunk_size.y - 1 + ((block_position.y + 1) % terrain_chunk_size.y);
    else positionl.y = block_position.y % terrain_chunk_size.y;
    if (block_position.z < 0) positionl.z = chunk_size.z - 1 + ((block_position.z + 1) % terrain_chunk_size.z);
    else positionl.z = block_position.z % terrain_chunk_size.z;
    return positionl;
}

static inline int3 voxel_chunk_position_xz(int3 chunk_position, int3 chunk_size) {
    return int3_multiply_int3(chunk_position, chunk_size);
}

static inline int3 get_chunk_block_position(int3 chunk_position, int3 chunk_size) {
    int3 block_position = int3_multiply_int3(chunk_position, chunk_size);
    return block_position;
}

float3 voxel_to_real_position(int3 block_position, float terrain_voxel_scale, float chunk_voxel_scale) {
    float3 positionf = int3_to_float3(block_position);
    float3_scale_p(&positionf, terrain_voxel_scale);
    // get middle of voxel position
    return float3_add(positionf, float3_scale(float3_halff, chunk_voxel_scale));
}

float3 local_block_position_to_real_position(byte3 local_position, int3 chunk_position, byte depth, float block_scale) {
    int3 chunk_block_position = get_chunk_block_position(chunk_position, int3_single(powers_of_two[depth]));
    int3 block_position = int3_add(chunk_block_position, byte3_to_int3(local_position));
    float3 positionf = int3_to_float3(block_position);
    float3_scale_p(&positionf, block_scale);
    // NOTE: Add half block
    positionf = float3_add(positionf, float3_single(block_scale / 2.0f));
    return positionf;
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

static inline float chunk_position_to_real_position1(int block_position, byte chunk_length, float scale) {
    return block_position * chunk_length * scale;
}

static inline float block_position_to_real_position1(int block_position, float scale) {
    return block_position * scale;
}

static inline float3 block_position_to_real_position(int3 block_position, float scale) {
    return (float3) {
        block_position_to_real_position1(block_position.x, scale),
        block_position_to_real_position1(block_position.y, scale),
        block_position_to_real_position1(block_position.z, scale)
    };
}

// Obsolete, this doesnt handle local depth differences
static inline byte3 get_positionl_byte3(int3 block_position, byte3 chunk_size) {
    byte3 positionl;
    if (block_position.x < 0) {
        positionl.x = chunk_size.x - 1 + ((block_position.x + 1) % chunk_size.x);
    } else {
        positionl.x = block_position.x % chunk_size.x;
    }
    if (block_position.y < 0) {
        positionl.y = chunk_size.y - 1 + ((block_position.y + 1) % chunk_size.y);
    } else {
        positionl.y = block_position.y % chunk_size.y;
    }
    if (block_position.z < 0) {
        positionl.z = chunk_size.z - 1 + ((block_position.z + 1) % chunk_size.z);
    } else {
        positionl.z = block_position.z % chunk_size.z;
    }
    return positionl;
}

// Map local-in-chunk coords (positionl) to world-voxel coords (block_position),
// scaling from the terrain's local basis (terrain_chunk_size) to this chunk's voxel size (chunk_size).
// Handles negative chunks the same way your get_positionl_* does: negative axes anchor to the
// high end of the chunk window.
//
// Assumes: 0 < terrain_chunk_size.{x,y,z} <= chunk_size.{x,y,z}
static inline int3 positionl_to_block_position(byte3 positionl, int3  chunk_position, int3  chunk_size, byte3 terrain_chunk_size) {
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

int3 chunk_position_to_block_position(int3 position, byte depth) {
    byte length = powers_of_two[depth];
    position.x = position.x * length;
    position.y = position.y * length;
    position.z = position.z * length;
    return position;
}

int3 block_position_to_chunk_position(int3 position, byte chunk_depth) {
    byte length = powers_of_two[chunk_depth];
    position.x = floor_div(position.x, length);
    position.y = floor_div(position.y, length);
    position.z = floor_div(position.z, length);
    return position;
    /*floor_div
    int3 block_position2 = block_position;
    if (block_position.x < 0) block_position2.x += 1;
    if (block_position.y < 0) block_position2.y += 1;
    if (block_position.z < 0) block_position2.z += 1;
    int3 chunk_position = int3_div1(block_position2, powers_of_two[chunk_depth]);*/
    // (int3) { block_position.x / chunk_size.x, block_position.y / chunk_size.y, block_position.z / chunk_size.z };
    // because for example -10 / 16 is 0 as an integer, but  coordinates we need a negative chunk position
    /*if (block_position.x < 0) chunk_position.x -= 1;
    if (block_position.y < 0) chunk_position.y -= 1;
    if (block_position.z < 0) chunk_position.z -= 1;
    return chunk_position;*/
    // return (int3) { block_position.x / chunk_size.x, block_position.y / chunk_size.y, block_position.z / chunk_size.z };
}

// Convert voxel-space coords (block_position) to local-in-chunk coords (positionl),
// scaling from this chunk's voxel size (chunk_size) to the terrain's local basis (terrain_chunk_size).
// Handles negative chunk positions with tail-window semantics (matches get_positionl_byte3_2).
static inline byte3 block_position_to_positionl(
    const int3  block_position,        // world voxel-space coords
    const int3  chunk_position,   // chunk grid coordinate
    const int3  chunk_size,       // voxel size of *this* chunk
    const byte3 terrain_chunk_size// terrain basis to scale to
) {
    // Origin voxel coord of this chunk
    const int3 base = int3_multiply_int3(chunk_position, chunk_size);

    // Offset inside the chunk in current chunk_size basis
    int3 local = int3_sub(block_position, base);

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
// Convert terrain-scale voxel coords (block_position) into terrain chunk coords (positionc)
// using terrain_chunk_size (NOT local chunk size).
static inline int3 block_position_to_positionc(
    const int3 block_position,         // voxel coords in terrain scale
    const byte3 terrain_chunk_size// voxel dimensions of a terrain chunk
) {
    int3 pos = block_position;

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
    if (block_position.x < 0) chunk_position.x -= 1;
    if (block_position.y < 0) chunk_position.y -= 1;
    if (block_position.z < 0) chunk_position.z -= 1;

    return chunk_position;
}
