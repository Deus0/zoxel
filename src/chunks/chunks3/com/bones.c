// a combined vox's voxel parts, so a skeleton mesh's vertices take their voxel's bone (doc/plans/skeletons.md)
//  - BoneNode: each voxel's part, its CombineList entry + 1 (0 air), written by combine_vox_system as it places them;
//    clothes (CombineOver) take the part they cover, or the nearest part past the body
//  - CombineBones: an entry's bone (its BoneLinks index), set by parts when it spawns the bones (clothes: their parent's)
typedef struct {
    void* ptr;
    byte value;
} BoneNode;

zoxc_octree_basic(BoneNode, byte, 0);
create_octree_setters(BoneNode);
create_octree_accessors(BoneNode);
create_octree_reducer(BoneNode);
create_octree_optimizer(BoneNode, 1);
zoxc_arrayd(CombineOver, byte);     // 1 if the entry covers what's under it (clothes, anchor over)
zoxc_arrayd(CombineBones, byte);    // an entry's bone index
