#define zox_voxel_queue_edit 0
#define zox_voxel_queue_post 1
#define zox_voxel_queue_end 2
typedef struct {
    byte3 position;
    byte value;
    byte data;          // the voxel's block data (BlockDatas), 0 for none
    byte old_value;
    byte old_data;      // its block data before, read after (a filler's offset to its base, multi_block_removed)
    byte state;
    byte flowed;        // set by flowing water (terrain/sys/water_flow.c): it doesn't start a flow of its own
} VoxelNodeUpdate;

zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1);
zoxc_queue_remove_at(VoxelNodeQueue, VoxelNodeUpdate);
