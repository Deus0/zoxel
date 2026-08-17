zox_tag(Bone);
zox_tag(Skeleton);
zox_tag(PaintedSkeleton);
zoxc_state(SkeletonDirty);
zoxc_float3(BonePosition);
zoxc_float3(BoneSize);
zoxc_arrayd(BoneIndexes, byte)
zoxc_arrayd(BoneLinks, entity)
zox_tag(HeadBone);
zox_tag(HipsBone);
zox_tag(ArmBone);
zox_tag(HandBone);
zox_tag(ShoulderBone);
zox_tag(ThighBone);
zoxc_entity(SkeletonLink);
zoxc_entity(HeadBoneLink);
zoxc_entity(HandBoneLink);
zoxc_entity(ShoulderBoneLink);
zoxc_byte(SwingState);
zoxc_byte(SwingAngle);
zoxc_byte(RaiseShoulder);
zoxc_byte(WalkState);
zoxc_byte(WalkSpeed);
zoxc_double(SwingStart);
zoxc_float(SwingSpeed);
#include "bone_index.c"

void define_components_bones(ecs* world) {
    zoxd_tag(Bone);
    zoxd_tag(Skeleton);
    zoxd_tag(PaintedSkeleton);
    zoxd_state(SkeletonDirty);
    zoxd_float3(BonePosition);
    zoxd_float3(BoneSize);
    zoxd(MaterialBone);
    zoxd_arrayd(BoneIndexes);
    zoxd_arrayd(BoneLinks);
    zoxd_tag(HeadBone);
    zoxd_tag(HipsBone);
    zoxd_tag(ArmBone);
    zoxd_tag(HandBone);
    zoxd_tag(ShoulderBone);
    zoxd_tag(ThighBone);
    zoxd_entity(SkeletonLink);
    zoxd_entity(HeadBoneLink);
    zoxd_entity(HandBoneLink);
    zoxd_entity(ShoulderBoneLink);
    zoxd_byte(SwingState);
    zoxd_byte(SwingAngle);
    zoxd_double(SwingStart);
    zoxd_float(SwingSpeed);
    zoxd_byte(RaiseShoulder);
    zoxd_byte(WalkState);
    zoxd_byte(WalkSpeed);
    zoxd_guint_dest(BoneIndexGPULink);
}
