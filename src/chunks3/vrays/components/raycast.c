typedef struct {
    byte result;
    byte voxel;
    entity hit_block;
    // current
    byte3 positionl;
    int3 positionv;
    float3 positionf;
    float voxel_scale;
    entity chunk;
    VoxelNode *node;
    float3 hit;
    float3 normal;
    float distance;
    // last
    entity chunk_last;
    VoxelNode *node_last;
    byte3 positionl_last;
    int3 positionv_last;
    float3 positionf_last;
} RaycastVoxelData;
zoxc_custom(RaycastVoxelData);