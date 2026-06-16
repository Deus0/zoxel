/// Map camera distance to a discrete LOD [0…max_lod], or invisible (255) if beyond view.
/// - distance: current chunk’s distance to camera
/// - depth: highest LOD index (e.g. 4 yields LODs 0,1,2,3,4)
/// - near: “near‑field” radius under which LOD 0 applies
/// - far: your global max view distance
// NOTE: near is a reserved words on windows..
static inline byte camera_distance_to_render_depth(byte distance, byte depth, byte nearf, byte farf) {
    // vanish beyond the horizon
    if (distance > farf) {
        // zox_log_error(" lod finder out of range: dist [%i] range [%i-%i] depth [%i]", distance, near, far, depth)
        return 0; // render_depth_invisible;
    }
    // carve the remaining range (render_distance–init_lod) into max_lod+1 equal slices
    // then find which slice “distance” lives in
    float total_range = (float) (farf - nearf);
    // +1 to avoid divide‑by‑zero when node_depth==0, and to have a small slice at the very end
    int slice = ceil(total_range / ((float) (depth))); //  + 1
    // each increment of i moves us one slice further out
    for (byte i = 0; i <= depth; i++) {
        byte threshold = nearf + (int)(slice * i); //  + 1
        if (distance <= threshold) {
            return depth - i;
        }
    }
    zox_log_error(" fallback lod finder: dist [%i] range [%i-%i] depth [%i] slice [%i]", distance, nearf, farf, depth, slice);
    // fallback (shouldn’t hit, but safe)
    return 0; // render_depth_invisible;
}

// Used for Lodding the Terrain Chunks
static inline byte camera_distance_to_terrain_render_depth(byte distance) {
    return camera_distance_to_render_depth(distance, terrain_depth, terrain_lod_near, terrain_lod_far);
}

static inline byte camera_distance_to_npc_render_depth(byte distance, byte mdepth) {
    if (zox_dbg_npc_all_max_depth) {
        return mdepth;
    }
    if (block_vox_depth < mdepth) {
        byte ddepth = (block_vox_depth_limits.y - block_vox_depth);
        mdepth = mdepth - ddepth < 0 ? 0 : mdepth - ddepth;
    }
    return camera_distance_to_render_depth(distance, mdepth, vox_lod_near, terrain_lod_near);
}

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

static inline byte camera_distance_to_block_vox_depth(byte distance) {
    return camera_distance_to_render_depth(distance, block_vox_depth, vox_lod_near, terrain_lod_near);
}
