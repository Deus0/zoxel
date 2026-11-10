zox_tag(Bone);
zox_tag(Skeleton);
zox_tag(PaintedSkeleton);
zoxc_dirty(Skeleton);
zoxc_float3(BoneSize);
zox_tag(HeadBone);
zox_tag(HipsBone);
zox_tag(ArmBone);
zox_tag(HandBone);
zox_tag(ShoulderBone);
zox_tag(ThighBone);
zox_tag(SkeletonLink);     // links to a skeleton, from its owner or its bones
zox_tag(SkeletonOwner);    // has a skeleton child, linked with SkeletonLink
zox_tag(HeadBoneLink);
zox_tag(HandBoneLink);
zox_tag(ShoulderBoneLink);
zoxc_byte(SwingState);
zoxc_byte(SwingAngle);
zoxc_float(SwingPhase);    // accumulated swing cycle, in radians
zoxc_float(SwingBlend);    // 0 idle -> 1 walking, smoothed
zoxc_byte(RaiseShoulder);
zoxc_byte(WalkState);
zoxc_byte(WalkSpeed);
zoxc_double(SwingStart);
zoxc_float(SwingSpeed);

void zox_components_bones(ecs* world) {
    zoxd_tag(Bone);
    zoxd_tag(Skeleton);
    zoxd_tag(PaintedSkeleton);
    zoxd_float3(BoneSize);
    zoxd_tag(HeadBone);
    zoxd_tag(HipsBone);
    zoxd_tag(ArmBone);
    zoxd_tag(HandBone);
    zoxd_tag(ShoulderBone);
    zoxd_tag(ThighBone);
    zoxd_nf_link(SkeletonLink);
    zoxd_tag(SkeletonOwner);
    zoxd_nf_tag(HeadBoneLink);
    zoxd_nf_tag(HandBoneLink);
    zoxd_nf_tag(ShoulderBoneLink);
    zoxd_byte(SwingState);
    zoxd_byte(SwingAngle);
    zoxd_float(SwingPhase);
    zoxd_float(SwingBlend);
    zoxd_double(SwingStart);
    zoxd_float(SwingSpeed);
    zoxd_byte(RaiseShoulder);
    zoxd_byte(WalkState);
    zoxd_byte(WalkSpeed);
    zoxd_dirty(Skeleton);
}
