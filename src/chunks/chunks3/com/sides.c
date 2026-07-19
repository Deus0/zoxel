// A simple struct
// TODO: Remove type from Macros, just use depth check?
typedef struct {
    void* ptr;
    byte value;
} SidesOctree;
zoxc_octree(SidesOctree, byte, 0);
// zoxc_state(SidesOctreeDirty);

new_octree_function_set(SidesOctree);
create_node_getter(SidesOctree);
create_octree_reducer(SidesOctree);
create_octree_optimizer(SidesOctree);
create_node_setreduce(SidesOctree);
create_node_neighbor(SidesOctree);

