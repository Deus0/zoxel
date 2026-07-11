typedef struct {
    byte result;
    byte voxel;
    entity hit_block;
    byte depth;
    // current
    byte3 positionl;
    int3 positionv;
    float3 positionf;
    float voxel_scale;
    entity chunk;
    VoxelNode* node;
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

static inline byte get_raycast_sides(ecs* world, const RaycastVoxelData* data) {
    zox_geter(data->chunk, SidesOctree, sides);
    return getv_SidesOctree(sides, data->depth, data->positionl);
}

void clear_raycast_data(RaycastVoxelData* data) {
    data->chunk = 0;
    data->positionf = float3_zero;
    data->positionl = byte3_zero;
    data->positionv = int3_zero;
    data->normal = float3_zero;
    data->hit = float3_zero;
    data->positionl_last = byte3_zero;
    data->positionv_last = int3_zero;
    data->positionf_last = float3_zero;
    data->chunk_last = 0;
    data->node = NULL;
    data->node_last = NULL;
}
