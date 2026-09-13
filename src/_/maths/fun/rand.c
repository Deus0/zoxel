static inline uint noise_rand(uint* state) {
    uint x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static inline byte noise_rand_range(
    uint* state,
    byte min,
    byte max)
{
    return min + (byte)(noise_rand(state) % ((uint) max - min + 1));
}