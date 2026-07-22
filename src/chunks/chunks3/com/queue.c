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

/*typedef struct {
    byte value;
    byte3 pos;
} VoxelDropElement;
zoxc_queue(VoxelDropQueue, VoxelDropElement, 1);
*/