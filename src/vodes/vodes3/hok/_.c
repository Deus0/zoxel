typedef struct {
    VoxelNode *node;
    const entity chunk;
    const byte block_index;
    const entity block;
    const byte3 positionl;
    const int3 positionv;
    const float3 positionf;
    const float scale;
    const byte render_disabled;
    const byte render_depth;
} spawned_block_data;

zox_hook(spawned_block, (ecs* world, spawned_block_data *data), (world, data))
