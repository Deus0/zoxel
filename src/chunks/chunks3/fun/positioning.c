const byte max_position_checks = 255;

int3 chunk_position_fix2(
    const float3 real_position,
    int3 chunk_position
) {
    if (real_position.x < 0) chunk_position.x -= 1;
    if (real_position.y < 0) chunk_position.y -= 1;
    if (real_position.z < 0) chunk_position.z -= 1;
    return chunk_position;
}

int3 real_position_to_chunk_position(
    float3 positionf,
    byte chunk_length,
    const float scale   // vox_scale
) {
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

static inline byte3 get_positionl_byte3_2(
    int3 positionv,
    byte3 chunk_size,
    byte3 terrain_chunk_size
) {
    byte3 positionl;
    if (positionv.x < 0) positionl.x = chunk_size.x - 1 + ((positionv.x + 1) % terrain_chunk_size.x);
    else positionl.x = positionv.x % terrain_chunk_size.x;
    if (positionv.y < 0) positionl.y = chunk_size.y - 1 + ((positionv.y + 1) % terrain_chunk_size.y);
    else positionl.y = positionv.y % terrain_chunk_size.y;
    if (positionv.z < 0) positionl.z = chunk_size.z - 1 + ((positionv.z + 1) % terrain_chunk_size.z);
    else positionl.z = positionv.z % terrain_chunk_size.z;
    return positionl;
}

static inline int3 voxel_chunk_position_xz(
    int3 chunk_position,
    int3 chunk_size
) {
    return int3_multiply_int3(chunk_position, chunk_size);
}

static inline int3 get_chunk_positionv(
    int3 chunk_position,
    int3 chunk_size
) {
    int3 positionv = int3_multiply_int3(chunk_position, chunk_size);
    return positionv;
}

float3 voxel_to_real_position(
    const int3 positionv,
    const float terrain_voxel_scale,
    const float chunk_voxel_scale
) {
    float3 positionf = int3_to_float3(positionv);
    float3_scale_p(&positionf, terrain_voxel_scale);
    // get middle of voxel position
    return float3_add(positionf, float3_scale(float3_halff, chunk_voxel_scale));
}

/*float3 positionv_to_real_position(
    const int3 positionv,
    const float voxel_scale
) {
    float3 position = int3_to_float3(positionv);
    float3_scale_p(&position, voxel_scale);
    // middle of voxel position
    position.x += voxel_scale / 2;
    position.y += voxel_scale / 2;
    position.z += voxel_scale / 2;
    return position;
}*/

// NOTE: this doesn't account for local chunk depth difference to terrain grid
float3 local_to_real_position_character(
    const byte3 in_chunk_position,
    const int3 chunk_grid_position,
    const float3 bounds,
    const float scale
) {
    // const float scale = get_terrain_voxel_scale(depth) * vox_scale;
    const int3 grid_position = int3_add(
        chunk_grid_position,
        byte3_to_int3(in_chunk_position));
    float3 position = int3_to_float3(grid_position);
    // zox_log("scale: %f", scale)
    float3_scale_p(&position, scale);
    position.x += scale / 2.0f;
    position.z += scale / 2.0f;
    position.y += bounds.y / 2.0f;
    position.y += 0.05f; // extra
    return position;
}
