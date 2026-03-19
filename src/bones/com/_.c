zox_tag(Bone);
zox_tag(Skeleton);
zox_tag(PaintedSkeleton);
zox_tag(HeadBone);
zoxc_float3(BonePosition);
zoxc_float3(BoneSize);
zoxc_arrayd(BoneIndexes, byte)
zoxc_arrayd(BoneLinks, entity)
zoxc_entity(SkeletonLink);
#include "bone_index.c"

void define_components_bones(ecs *world) {
    zoxd_tag(Bone)
    zoxd_tag(Skeleton)
    zoxd_tag(PaintedSkeleton)
    zoxd_tag(HeadBone)
    zoxd_float3(BonePosition)
    zoxd_float3(BoneSize)
    zoxd(MaterialBone)
    zoxd_arrayd(BoneIndexes)
    zoxd_arrayd(BoneLinks)
    zox_define_component_w_dest(BoneIndexGPULink)
    zoxd_entity(SkeletonLink);
}
