typedef struct {
    const entity realm;
    BlockLinks *blocks;
} spawn_blocks_data;
// todo: move this to core voxels module
zox_hook(spawn_blocks, (ecs* world, spawn_blocks_data *data), (world, data))