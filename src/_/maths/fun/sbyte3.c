// NOTE: Signed version of bytes!
const sbyte3 sbyte3_zero = (sbyte3) { 0, 0, 0 };

static const sbyte3 neighbor_offsets[6] = {
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1},
};

static inline sbyte3 sbyte3_add(sbyte3 a, sbyte3 b) {
    return (sbyte3) { a.x + b.x, (sbyte) a.y + b.y, (sbyte) a.z + b.z };
}

static inline byte3 sbyte3_to_byte3(sbyte3 a) {
    return (byte3) { (byte) a.x, (byte) a.y, (byte) a.z };
}

#define OCTREE_INDEX(x, y, z) (((z) + 1) * 9 + ((y) + 1) * 3 + ((x) + 1))

#define OCTREE_INDEX3(pos) (((pos.z) + 1) * 9 + ((pos.y) + 1) * 3 + ((pos.x) + 1))

// x,y,z in {-1,0,+1}
static inline byte nearby_position_valid(sbyte3 position) {
    return !(position.x < -1 || position.x > 1 ||
    position.y < -1 || position.y > 1 ||
    position.z < -1 || position.z > 1);
}

// NOTE: 1u << 8 gives 256 size
static inline ushort octree_size(byte depth) {
    return 1u << depth;
}
