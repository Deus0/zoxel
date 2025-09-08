typedef struct {
    void* ptr;
    byte value;
    byte type;          // 0 closed - 1 open - 2 node link
    byte sides;         // new field for lighting/mesh culling
} VoxelNode;

zoxc_octree_fun1(VoxelNode, byte, 0)
zoxc_octree_fun2(VoxelNode, byte)
zox_node_add_link(VoxelNode)


create_node_setter(VoxelNode)
create_node_getter(VoxelNode)

create_node_reducer(VoxelNode)
create_node_setreduce(VoxelNode)
create_node_neighbor(VoxelNode)
create_octree_line_debugger(VoxelNode)

zoxc_byte(VoxelNodeDirty);
zoxc_byte(VoxelNodeEdited);
zoxc_byte(VoxelNodeLoaded);
zoxc_state(VoxelNodeGenerated);

zoxc_queue(VoxelNodeQueue, VoxelNodeUpdate, 1)