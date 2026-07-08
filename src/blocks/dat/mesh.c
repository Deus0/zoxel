static const float2 voxel_face_uvs[6][4] = {
    // left (-X)  (use Z horizontal, Y vertical)
    {
        {0,0}, // 0: (0,0,0)
        {1,0}, // 1: (0,0,1)
        {1,1}, // 2: (0,1,1)
        {0,1}  // 3: (0,1,0)
    },
    // right (+X) (use Z horizontal, Y vertical)
    {
        {1,0}, // 0: (1,0,0)
        {0,0}, // 1: (1,0,1)
        {0,1}, // 2: (1,1,1)
        {1,1}  // 3: (1,1,0)
    },
    // down (-Y) (use X horizontal, Z vertical)
    {
        {0,1}, // 0: (0,0,0)
        {0,0}, // 1: (0,0,1)
        {1,0}, // 2: (1,0,1)
        {1,1}  // 3: (1,0,0)
    },
    // up (+Y) (use X horizontal, Z vertical)
    {
        {0,0}, // 0: (0,1,0)
        {0,1}, // 1: (0,1,1)
        {1,1}, // 2: (1,1,1)
        {1,0}  // 3: (1,1,0)
    },
    // back (-Z) (use X horizontal, Y vertical)
    {
        {1,0}, // 0: (0,0,0)
        {1,1}, // 1: (0,1,0)
        {0,1}, // 2: (1,1,0)
        {0,0}  // 3: (1,0,0)
    },
    // front (+Z) (use X horizontal, Y vertical)
    {
        {0,0}, // 0: (0,0,1)
        {0,1}, // 1: (0,1,1)
        {1,1}, // 2: (1,1,1)
        {1,0}  // 3: (1,0,1)
    }
};


// left
static const float3 voxel_face_vertices_0[] = {
    { 0, 0, 0 },
    { 0, 0, 1 },
    { 0, 1, 1 },
    { 0, 1, 0 },
};

// right
static const float3 voxel_face_vertices_1[] = {
    { 1, 0, 0 },
    { 1, 0, 1 },
    { 1, 1, 1 },
    { 1, 1, 0 },
};

// down
static const float3 voxel_face_vertices_2[] = {
    { 0, 0, 0 },
    { 0, 0, 1 },
    { 1, 0, 1 },
    { 1, 0, 0 },
};

// up
static const float3 voxel_face_vertices_3[] = {
    { 0, 1, 0 },
    { 0, 1, 1 },
    { 1, 1, 1 },
    { 1, 1, 0 },
};

// back
static const float3 voxel_face_vertices_4[] = {
    { 0, 0, 0 },
    { 0, 1, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
};

// front
static const float3 voxel_face_vertices_5[] = {
    { 0, 0, 1 },
    { 0, 1, 1 },
    { 1, 1, 1 },
    { 1, 0, 1 },
};

static const float3* voxel_face_vertices_n[] = {
    voxel_face_vertices_0,
    voxel_face_vertices_1,
    voxel_face_vertices_2,
    voxel_face_vertices_3,
    voxel_face_vertices_4,
    voxel_face_vertices_5
};

static const int voxel_face_indicies_n[] = {
    // X
    0, 1, 2, 2, 3, 0,   // normal
    2, 1, 0, 0, 3, 2,   // reversed
    // Y
    2, 1, 0, 0, 3, 2,   // reversed
    0, 1, 2, 2, 3, 0,   // normal
    // Z
    0, 1, 2, 2, 3, 0,   // normal
    2, 1, 0, 0, 3, 2,   // reversed
};