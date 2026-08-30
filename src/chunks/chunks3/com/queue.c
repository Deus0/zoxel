#define zox_voxel_queue_edit 0
#define zox_voxel_queue_post 1
#define zox_voxel_queue_end 2
typedef struct {
    byte3 position;
    byte value;
    byte old_value;
    byte state;
} VoxelNodeUpdate;

zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1);
zoxc_queue_remove_at(VoxelNodeQueue, VoxelNodeUpdate);
