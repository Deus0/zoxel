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
    return min + (seed_rand(seed) % (max - min));
}
