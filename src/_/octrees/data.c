#define octree_length 8
#define node_type_closed 0
#define node_type_children 0
#define node_type_instance 255
byte dbg_log_octree_errors = 1;

typedef struct {
    void* ptr;
    byte value;
} OctreeBasic;

typedef struct {
    entity value;
} OctreeLink;

typedef struct {
    byte opposite;
    byte axis;
    byte side;
} SideInfo;

// NOTE: New ordering is XYZ instead of old ZYX
static const byte3 octree_positions[] = {
    {0,0,0},
    {1,0,0},
    {0,1,0},
    {1,1,0},
    {0,0,1},
    {1,0,1},
    {0,1,1},
    {1,1,1}
};

// NOTE: We now use XYZ ordering
static inline byte byte3_octree_array_index(byte3 position) {
    return  (position.x) |
            (position.y << 1) |
            (position.z << 2);
}

static const SideInfo side_infos[6] = {
    {1, 0, 0}, // -x sees +x neighbor
    {0, 0, 1}, // +x sees -x neighbor
    {3, 1, 0}, // -y
    {2, 1, 1}, // +y
    {5, 2, 0}, // -z
    {4, 2, 1}  // +z
};

static const byte3 side_offsets[6] = {
    {-1,0,0},
    { 1,0,0},
    {0,-1,0},
    {0, 1,0},
    {0,0,-1},
    {0,0, 1}
};
static byte has_side_child_indices = 0;
static byte side_child_indices[3][2][4];

static inline void init_side_child_indices() {
    if (has_side_child_indices) {
        return;
    }
    has_side_child_indices = 1;
    for (byte axis = 0; axis < 3; axis++) {
        for (byte side = 0; side < 2; side++) {
            byte index = 0;
            for (byte i = 0; i < 8; i++) {
                byte child_side = (i >> axis) & 1;
                if (child_side == side) {
                    side_child_indices[axis][side][index++] = i;
                }
            }
        }
    }
}
