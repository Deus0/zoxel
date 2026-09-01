typedef struct {
    void* ptr;
    byte value;
    byte type;          // 0 closed - 1 open - 2 node link
} VoxelNode;

zoxc_octree_linked(VoxelNode, byte, 0);
// zoxc_octree_fun1(VoxelNode, byte, 0);
zoxc(VoxelNodeLock, spinlock);

create_octree_setters(VoxelNode);
create_octree_accessors(VoxelNode);
zoxc_octree_fun2(VoxelNode, byte);
zox_node_add_link(VoxelNode);
create_octree_optimizer(VoxelNode, offsetof(VoxelNode, type));
create_octree_reducer(VoxelNode, offsetof(VoxelNode, type));
create_node_neighbor(VoxelNode);
create_octree_line_debugger(VoxelNode);
