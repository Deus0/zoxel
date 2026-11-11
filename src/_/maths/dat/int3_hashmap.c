// NOTE: packed, so keys are unique within a 4096 x 256 x 4096 window (hashmaps only store the hash)
//  - the old linear hash collided at [0,1,-139] apart
static inline uint get_int3_hash(int3 input) {
    return (((uint) input.x & 0xfff) << 20) |
        (((uint) input.y & 0xff) << 12) |
        ((uint) input.z & 0xfff);
}
