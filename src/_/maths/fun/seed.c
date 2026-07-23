static inline uint seed_rand(uint seed) {
    seed ^= seed >> 16;
    seed *= 0x7feb352d;
    seed ^= seed >> 15;
    seed *= 0x846ca68b;
    seed ^= seed >> 16;
    return seed;
}

// NOTE: Max is Exclusive (node including max)
static inline int seed_range(uint seed, int min, int max) {
#ifdef zox_safety_checks
    if (min >= max) {
        zox_loge("Invalid seed range [%i:%i]", min, max);
        return 0;
    }
#endif
    return min + (seed_rand(seed) % (max - min));
}

static inline uint position_seed(uint base_seed, int3 position) {
    uint seed = base_seed;
    seed ^= (uint) position.x * 0x9E3779B1u;
    seed ^= (uint) position.y * 0x85EBCA77u;
    seed ^= (uint) position.z * 0xC2B2AE3Du;
    return seed_rand(seed);
}

static inline uint position_seed2(uint base_seed, int2 position) {
    uint seed = base_seed ^ 0x4B1D5A37u; // 2D domain tag
    seed ^= (uint)position.x * 0x9E3779B1u;
    seed ^= (uint)position.y * 0x85EBCA77u;
    return seed_rand(seed);
}
