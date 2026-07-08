
static const float2 voxel_face_uvs[6][4] = {
    // left  (-X)  U = +Z, V = +Y
    {
        {0, 0}, // (0,0,0)
        {1, 0}, // (0,0,1)
        {1, 1}, // (0,1,1)
        {0, 1}, // (0,1,0)
    },

    // right (+X) indices flipped: 2,1,0,0,3,2
    // flip U
    {
        {0, 0}, // (1,0,0)
        {1, 0}, // (1,0,1)
        {1, 1}, // (1,1,1)
        {0, 1}, // (1,1,0)
    },

    // down (-Y) indices flipped: 0,1,2,2,3,0
    // flip V
    {
        {0, 0}, // (0,0,0)
        {0, 1}, // (0,0,1)
        {1, 1}, // (1,0,1)
        {1, 0}, // (1,0,0)
    },

    // up    (+Y)  U = +X, V = +Z
    {
        {0, 0}, // (0,1,0)
        {0, 1}, // (0,1,1)
        {1, 1}, // (1,1,1)
        {1, 0}, // (1,1,0)
    },

    // back  (-Z)  U = +X, V = +Y
    {
        {0, 0}, // (0,0,0)
        {0, 1}, // (0,1,0)
        {1, 1}, // (1,1,0)
        {1, 0}, // (1,0,0)
    },

    // front (+Z) indices flipped: 0,1,2,2,3,0
    {
        {0, 0}, // (0,0,0)
        {0, 1}, // (0,1,0)
        {1, 1}, // (1,1,0)
        {1, 0}, // (1,0,0)
    },
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

// One consistent triangle winding for every face.
// If your renderer culls the wrong side, flip these two triangles.
static const int voxel_face_indicies_n[] = {
    // -X left (normal points -X)
    0, 1, 2,  2, 3, 0,
    // +X right (normal points +X)
    2, 1, 0,  0, 3, 2,
    // -Y down (normal points -Y)
    2, 1, 0,  0, 3, 2,
    // +Y up (normal points +Y)
    0, 1, 2,  2, 3, 0,
    // -Z back (normal points -Z)
    0, 1, 2,  2, 3, 0,
    // +Z front (normal points +Z)
    2, 1, 0,  0, 3, 2,
};