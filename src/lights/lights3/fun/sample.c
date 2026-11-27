// the light at a point in a chunk, 0 to 1 (light / 255, as terrain vertex colours): its voxel's, under the chunk's
//  light lock (flood passes write it on threads); under 0 if it has none there (no lights, the point outside it)
static inline float chunk_light_at(ecs* world, entity chunk, float3 position) {
    if (zox_no_lights || !zox_valid(chunk) || !zox_has(chunk, LightOctree) || !zox_has(chunk, LightLock)) {
        return -1;
    }
    const float scale = zox_getv(chunk, BlockScale);    // its voxels' size at its depth
    const byte depth = zox_getv(chunk, NodeDepth);
    const float3 origin = zox_getv(chunk, Position3D);  // its corner
    const int size = octree_size(depth);
    const int3 local = {
        (int) floorf((position.x - origin.x) / scale),
        (int) floorf((position.y - origin.y) / scale),
        (int) floorf((position.z - origin.z) / scale)
    };
    if (scale <= 0 || local.x < 0 || local.y < 0 || local.z < 0 || local.x >= size || local.y >= size || local.z >= size) {
        return -1;
    }
    LightLock* lock = zox_mut(chunk, LightLock);
    spin_lock(&lock->value);
    const byte light = getv_LightOctree(zox_get(chunk, LightOctree), depth, (byte3) { local.x, local.y, local.z });
    spin_unlock(&lock->value);
    return light / 255.0f;
}
