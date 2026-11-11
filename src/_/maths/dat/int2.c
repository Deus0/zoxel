/*#define int2_zero (int2) { 0, 0 }
#define int2_one (int2) { 1, 1 }
#define int2_two (int2) { 2, 2 }
#define int2_four (int2) { 4, 4 }
#define int2_hidden (int2) { -666, -666 }*/

// =====================================
// int2 constants
// =====================================
static const int2 int2_zero   = { 0, 0 };
static const int2 int2_one    = { 1, 1 };
static const int2 int2_two    = { 2, 2 };
static const int2 int2_four   = { 4, 4 };
static const int2 int2_hidden = { -666, -666 };

// NOTE: packed, so keys are unique within a 65536 x 65536 window (hashmaps only store the hash)
//  - the old linear hash collided at [1,-109] apart
static inline uint get_int2_hash(int2 input) {
    return (((uint) input.x & 0xffff) << 16) |
        ((uint) input.y & 0xffff);
}