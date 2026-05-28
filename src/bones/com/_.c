zox_tag(Bone);
zox_tag(Skeleton);
zox_tag(PaintedSkeleton);
zox_tag(HeadBone);
zoxc_state(SkeletonDirty);
zoxc_float3(BonePosition);
zoxc_float3(BoneSize);
zoxc_arrayd(BoneIndexes, byte)
zoxc_arrayd(BoneLinks, entity)
#include "bone_index.c"
zox_tag(HeadBone);
zox_tag(HandBone);
zox_tag(ShoulderBone);
zoxc_entity(SkeletonLink);
zoxc_entity(HeadBoneLink);
zoxc_entity(HandBoneLink);
zoxc_entity(ShoulderBoneLink);
zoxc_byte(RaiseShoulder);

void define_components_bones(ecs* world) {
    zoxd_tag(Bone);
    zoxd_tag(Skeleton);
    zoxd_tag(PaintedSkeleton);
    zoxd_tag(HeadBone);
    zoxd_state(SkeletonDirty);
    zoxd_float3(BonePosition);
    zoxd_float3(BoneSize);
    zoxd(MaterialBone);
    zoxd_arrayd(BoneIndexes);
    zoxd_arrayd(BoneLinks);
    zoxd_dest(BoneIndexGPULink);
    zoxd_tag(HeadBone);
    zoxd_tag(HandBone);
    zoxd_tag(ShoulderBone);
    zoxd_entity(SkeletonLink);
    zoxd_entity(HeadBoneLink);
    zoxd_entity(HandBoneLink);
    zoxd_entity(ShoulderBoneLink);
    zoxd_byte(RaiseShoulder);
}
