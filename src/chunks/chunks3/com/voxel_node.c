typedef struct {
    void* ptr;
    byte value;
    byte type;          // 0 closed - 1 open - 2 node link
} VoxelNode;

typedef struct {
    byte value;
    byte3 pos;
} VoxelNodeUpdate;

new_octree_function_set(VoxelNode);
new_octree_function_set_clean(VoxelNode);
create_node_getter(VoxelNode);
zoxc_octree_fun1(VoxelNode, byte, 0);
zoxc_octree_fun2(VoxelNode, byte);
zox_node_add_link(VoxelNode);
create_octree_optimizer_linked(VoxelNode);
create_octree_reducer_linked(VoxelNode);
create_node_setreduce(VoxelNode);
create_node_neighbor(VoxelNode);
create_octree_line_debugger(VoxelNode);
zoxc_byte(VoxelNodeDirty);
zoxc_byte(VoxelNodeEdited);
// zoxc_byte(VoxelNodeLoaded);
// zoxc_state(VoxelNodeGenerated);
zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1);
