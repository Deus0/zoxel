// A simple struct
// TODO: Remove type from Macros, just use depth check?
typedef struct {
    void* ptr;
    byte value;
    byte type;
} SidesOctree;

zoxc_octree_fun1(SidesOctree, byte, 0);
create_node_setter(SidesOctree);
create_node_getter(SidesOctree);
create_octree_reducer(SidesOctree);
create_octree_optimizer(SidesOctree);
create_node_setreduce(SidesOctree);
create_node_neighbor(SidesOctree);
zoxc_state(SidesOctreeDirty);
