// using primes for unique hash
#define hash_constant_byte 37
#define hash_multiple_1 29 // 163
#define hash_multiple_2 31 // 109
#define hash_multiple_3 37 // 139

// NOTE: packed, every byte3 has its own key
uint get_byte3_hash(byte3 input) {
    return ((uint) input.x << 16) |
        ((uint) input.y << 8) |
        (uint) input.z;
}