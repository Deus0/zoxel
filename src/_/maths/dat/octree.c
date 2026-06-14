static const byte3 octree_node_size4 = { 2, 2, 2 };

static const float3 octree_positions_float3s[] = {
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f }
};

static const byte3 octree_positions_b[] = {
    { 0, 0, 0},
    { 0, 0, 1},
    { 0, 1, 0},
    { 0, 1, 1},
    { 1, 0, 0},
    { 1, 0, 1},
    { 1, 1, 0},
    { 1, 1, 1}
};

const float octree_scales[] = {
    1.0f,
    0.5f,
    0.25f,
    0.125f,
    0.0625f,
    0.03125f,
    0.015625f,
    0.0078125f
};

const float octree_scales2[] = {
    // 2.0f,
    1.0f,
    0.5f,
    0.25f,
    0.125f,
    0.0625f,
    0.03125f,
    0.015625f,
    0.0078125f
};

const float octree_scales3[] = {
    // 2.0f,
    1.0f,
    0.5f,
    0.25f,
    0.125f,
    0.0625f,
    0.03125f,
    0.015625f,
    0.0078125f
};

#define octree_length 8
// #define octree_node_size 2
const byte octree_node_size = 2;
#define octree_node_size3 (int3) { 2, 2, 2 }



const int3 octree_positions[] = {
    { 0, 0, 0},
    { 0, 0, 1},
    { 0, 1, 0},
    { 0, 1, 1},
    { 1, 0, 0},
    { 1, 0, 1},
    { 1, 1, 0},
    { 1, 1, 1}
};

const byte node_index_with_down[] = {
    -1,
    -1,
    0,
    1,
    -1,
    -1,
    4,
    5
};

const byte node_index_with_up[] = {
    2,
    3,
    -1,
    -1,
    6,
    7,
    -1,
    -1
};

const byte node_index_with_back[] = {
    -1,
    0,
    -1,
    2,
    -1,
    4,
    -1,
    6
};

const byte node_index_with_front[] = {
    1,
    -1,
    3,
    -1,
    5,
    -1,
    7,
    -1
};

const byte node_index_with_left[] = {
    -1,
    -1,
    -1,
    -1,
    0,
    1,
    2,
    3
};

const byte node_index_with_right[] = {
    4,
    5,
    6,
    7,
    -1,
    -1,
    -1,
    -1
};

// NOTE: For some reason my octree faces are different axis
// converts face to octree indexes
static const byte octree_face_children[6][4] = {
    {4, 5, 6, 7}, // +X
    {0, 1, 2, 3}, // -X
    {2, 3, 6, 7}, // +Y
    {0, 1, 4, 5}, // -Y
    {1, 3, 5, 7}, // +Z
    {0, 2, 4, 6}, // -Z
};
