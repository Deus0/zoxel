// A simple struct
// TODO: Remove type from Macros, just use depth check?
typedef struct {
    void* ptr;
    byte value;
} SidesOctree;

zoxc_octree_basic(SidesOctree, byte, 0);
create_octree_setters(SidesOctree);
create_octree_accessors(SidesOctree);
create_octree_reducer(SidesOctree, 0);
create_octree_optimizer(SidesOctree, 0);
create_node_neighbor(SidesOctree);
zoxc(SidesOctreeLock, spinlock);

