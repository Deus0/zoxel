zox_tag(Bone);
zox_tag(Skeleton);
zox_tag(PaintedSkeleton);
zoxc_dirty(Skeleton);
zoxc_float3(BoneSize);
zoxc_float3(RestPosition3D);    // on a bone the rig moves: its LocalPosition3D at rest (a chest leaning, ik/sys/spine.c; a shoulder reaching, ik/sys/arm_ik.c)
zoxc_float3(BoneCentre);    // its part's middle, the skeleton's space at rest (BonePosition is its pivot, the joint)
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
zox_tag(OffShoulderBoneLink);      // the left shoulder, the off hand: its guard and recoil in a punch (ik/sys/swing.c)
zoxc_byte(SwingState);
zoxc_byte(SwingAngle);
zoxc_float(SwingPhase);    // accumulated swing cycle, in radians
zoxc_float(SwingBlend);    // 0 idle -> 1 walking, smoothed
zoxc_byte(RaiseShoulder);
zoxc_byte(WalkState);
zoxc_byte(CrouchState);     // on a skeleton: crouching, bit 1 held (a key), bit 2 toggled (a button): bones/sys/arm_control.c
zoxc_float(CrouchBlend);    // 0 standing -> 1 crouched, smoothed
zoxc_byte(WalkSpeed);
zoxc_double(SwingStart);
zoxc_float(SwingSpeed);
zoxc_byte(SwingKind);       // the swing SwingStart starts, one of swings (dat/swings.c)
zoxc_byte(SwingMark);       // the mark a swing just passed (1 its first: a punch's hit, a bite): set as it passes, observers react
zoxc_float(SwingTime);      // on an action (a skill, an item): its swing's length, seconds (its attack speed), the hit at its first mark
zoxc_entity(SwingAction);   // what's swung: a skill or item (its hit's damage, eating's food), observers of the marks read it
zoxc_float3(SwingTarget);   // the point in the world a swing is at (a place's block: the head turns to it), zero none
zoxc_float(SpineTwist);     // the upper body turned about its up, radians (a punch turning into its strike)
zoxc_float(SpineTwistHead); // the twist the head was turned back by, last set (its look holds, ik/sys/spine.c)
zoxc_entity(LookTarget);
zoxc_entity(BoneSlot);      // the slot a bone is from: a rebuild keeps its index (parts/sys/bones.c)
#define zox_bones_max 32    // bone_matrix[32] in res/shaders/bone.vert    // on a head bone: it turns to look at it (its head, if it has one), 0 looks ahead

void zox_components_bones(ecs* world) {
    zoxd_tag(Bone);
    zoxd_tag(Skeleton);
    zoxd_tag(PaintedSkeleton);
    zoxd_float3(BoneSize);
    zoxd_float3(BoneCentre);
    zoxd_float3(RestPosition3D);
    zoxd_tag(HeadBone);
    zoxd_tag(HipsBone);
    zoxd_tag(ArmBone);
    zoxd_tag(HandBone);
    zoxd_tag(ShoulderBone);
    zoxd_tag(ThighBone);
    zoxd_nf_link(SkeletonLink);
    zoxd_tag(SkeletonOwner);
    zoxd_nf_link(HeadBoneLink);
    zoxd_nf_link(HandBoneLink);
    zoxd_nf_link(ShoulderBoneLink);
    zoxd_nf_link(OffShoulderBoneLink);
    zoxd_byte(SwingState);
    zoxd_byte(SwingAngle);
    zoxd_float(SwingPhase);
    zoxd_float(SwingBlend);
    zoxd_double(SwingStart);
    zoxd_float(SwingSpeed);
    zoxd_byte(SwingKind);
    zoxd_float3(SwingTarget);
    zoxd_entity(SwingAction);
    zoxd_float(SwingTime);
    zoxd_byte(SwingMark);
    zoxd_float(SpineTwist);
    zoxd_float(SpineTwistHead);
    zoxd_entity(LookTarget);
    zoxd_entity(BoneSlot);
    zoxd_byte(RaiseShoulder);
    zoxd_byte(WalkState);
    zoxd_byte(CrouchState);
    zoxd_float(CrouchBlend);
    zoxd_byte(WalkSpeed);
    zoxd_dirty(Skeleton);
}
