#define node_type_closed 0
#define node_type_children 0
#define node_type_instance 255

#define voxel_node_type_closed 0    // air / free
#define voxel_node_type_children 0  // pointing to 8 VoxelNode children
#define voxel_node_type_basic 2     // byte representing a voxel
#define voxel_node_type_entity 3

// We should just check ptr if opened or closed, type should stay for spawned entities!