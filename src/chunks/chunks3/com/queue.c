typedef struct {
    byte value;
    byte3 pos;
} VoxelNodeUpdate;
zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1);

typedef struct {
    byte value;
    byte3 pos;
} VoxelDropElement;
zoxc_queue(VoxelDropQueue, VoxelDropElement, 1);
