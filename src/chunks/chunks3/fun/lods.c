/// Map camera distance to a discrete LOD [0…max_lod], or invisible (255) if beyond view.
/// - distance: current chunk’s distance to camera
/// - depth: highest LOD index (e.g. 4 yields LODs 0,1,2,3,4)
/// - near: “near‑field” radius under which LOD 0 applies
/// - far: your global max view distance
// NOTE: near is a reserved words on windows..
static inline byte camera_distance_to_render_depth(byte distance,byte depth, byte nearf, byte farf) {
    // vanish beyond the horizon
    if (distance > farf) {
        // zox_log_error(" lod finder out of range: dist [%i] range [%i-%i] depth [%i]", distance, near, far, depth)
        return render_depth_invisible;
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
    return render_depth_invisible;
}

static inline byte camera_distance_to_terrain_render_depth(byte distance) {
    return camera_distance_to_render_depth(distance, terrain_depth, terrain_lod_near, terrain_lod_far);
}

static inline byte camera_distance_to_npc_render_depth(byte distance, byte max_render_depth) {
    return camera_distance_to_render_depth(distance, max_render_depth, vox_lod_near, terrain_lod_near);
}
