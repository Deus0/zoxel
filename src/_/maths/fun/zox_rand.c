// rand with its own state each thread (xorshift): libc's rand has one state for every thread, so a model
// generator seeding it then drawing on a worker was changed by another's draws, and one seed made different worlds
//  - zox_srand(seed) then zox_rand() on a thread gives the same numbers whatever other threads do
//  - in [0, RAND_MAX] like rand (32767 on windows)
static __thread uint zox_rand_state = 2463534242u;

static inline void zox_srand(uint seed) {
    zox_rand_state = seed ? seed : 2463534242u;
}

static inline int zox_rand() {
    uint x = zox_rand_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_rand_state = x;
    return (int) (x % ((uint) RAND_MAX + 1));
}
