typedef struct {
    byte *solidity;
    int *uvs;
} chunk3_textured_builder_data;

typedef struct {
    int_array_d *indicies;
    float3_array_d* vertices;
    float2_array_d* uvs;
    color_rgb_array_d* color_rgbs;
} mesh_uvs_build_data;

typedef struct {
    const int* indicies;
    const float3 *vertices;
    const float2 *uvs;
} octree_face_data;
