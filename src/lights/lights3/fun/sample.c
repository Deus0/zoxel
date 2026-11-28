// a channel's light at a point in a chunk (zox_light_sky, zox_light_block), 0 to 1 (light / 255, as terrain vertex
//  colours): its voxel's, under the chunk's light lock (flood passes write it on threads); under 0 if it has none
//  there (no lights, the point outside it)
static inline float chunk_light_at(ecs* world, entity chunk, float3 position, byte channel) {
    if (!zox_lighting || !zox_valid(chunk) || !zox_has(chunk, LightOctree) || !zox_has(chunk, LightLock)) {
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
    const LightOctree* lights = chunk_lights(world, chunk, channel);
    const byte light = lights ? getv_LightOctree(lights, depth, (byte3) { local.x, local.y, local.z }) : 0;
    spin_unlock(&lock->value);
    return light / 255.0f;
}

// the brighter of a chunk's sky and blocks' light at a voxel, at its depth (no lock: the main thread, tests); 0
//  without lights. One tree held both before the split (doc/plans/day_night.md), so it reads as that did
static inline byte chunk_light_brightest(ecs* world, entity chunk, byte depth, byte3 position) {
    const LightOctree* sky = chunk_lights(world, chunk, zox_light_sky);
    const LightOctree* block = chunk_lights(world, chunk, zox_light_block);
    const byte light = sky ? getv_LightOctree(sky, depth, position) : 0;
    const byte glow = block ? getv_LightOctree(block, depth, position) : 0;
    return glow > light ? glow : light;
}
