typedef struct {
    byte value;
    byte3 pos;
} VoxelNodeUpdate;
zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1);
