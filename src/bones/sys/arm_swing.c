// The walk's limbs, in step: each posed from its skeleton's SwingPhase and SwingBlend (arm_control.c)
//  - legs swing from the hips (SwingAngle each way), the one swinging ahead bends its knee to lift its foot, straight
//    again as it lands; arms swing against them, elbows a little bent, more as the arm comes ahead
//  - standing (SwingBlend 0) the legs are still, the arms sway a little
//  - arms slerp towards their target, smoothing any other snaps (i.e. shoulder raise ending), legs are on it (the
//    blend eases them); the elbows and knees are the walk's while it swings their limb (arm_ik, leg_ik leave them be
//    at weight 0)
//  - x turns a limb: less ahead, more back (a shin's more bends its knee, a forearm's less its elbow)
#define walk_follow_rate 18.0f     // how fast an arm slerps to its swing
#define walk_idle_arm_angle 5.0f    // degrees each way, standing
#define walk_knee_lift 55.0f        // the knee's most bend, mid swing ahead
#define walk_knee_bend 4.0f         // and always, walking
#define walk_elbow_bend 12.0f       // the elbow's bend, walking
#define walk_elbow_lift 18.0f       // more as the arm swings ahead

// a bone's child bone, along its limb (a thigh's shin, a shin's foot)
static inline entity bone_child_bone(ecs* world, entity bone) {
    return zox_get_child_by_id(world, bone, zox_id(Bone));
}

static inline void walk_bend(ecs* world, entity bone, float degrees, float t) {
    if (!zox_valid(bone)) {
        return;
    }
    LocalRotation3D* rotation = zox_getm(bone, LocalRotation3D);
    const float4 target = euler_to_quaternion((float3) { degrees * degrees_to_radians, 0, 0 });
    rotation->value = quaternion_normalize2(quaternion_slerp(rotation->value, target, t));
}

// a limb's walk pose this frame: phase and blend its skeleton's (arm_control.c), limb 1 a leg
//  - posed by arm_control_system with its skeleton, on its thread: as systems of their own the limbs were split
//    across workers and some read the skeleton's phase before it moved (one leg a frame behind the other)
static inline void walk_limb(ecs* world, entity e, float phase, float blend, byte limb, float follow_t) {
    const float idle_angle = limb ? 0 : walk_idle_arm_angle;
    const float angle = idle_angle + ((float) zox_getv(e, SwingAngle) - idle_angle) * blend;
    // its swing (sine) and which way it's going (cosine): opposite sides swing opposite ways, legs against arms
    float sign = zox_getv(e, LocalPosition3D).x < 0 ? -1 : 1;
    if (limb) {
        sign = -sign;
    }
    const float swing = sinf(phase) * sign;
    const float ahead = -cosf(phase) * sign;    // > 0 swinging ahead (x falling)
    const float4 target = euler_to_quaternion((float3) { swing * angle * degrees_to_radians, 0, 0 });
    // legs on their swing, not eased: the body bobs from it this frame (arm_control.c), eased they lagged it (at
    //  10 frames a second a whole step) and the stance foot rose and sank 8cm
    const float t = limb ? 1 : follow_t;
    LocalRotation3D* rotation = zox_getm(e, LocalRotation3D);
    rotation->value = quaternion_normalize2(quaternion_slerp(rotation->value, target, t));
    const entity child = bone_child_bone(world, e);
    if (limb) {
        // the knee: its shin
        walk_bend(world, child, blend * (walk_knee_bend + walk_knee_lift * fmaxf(0, ahead)), t);
    } else {
        // the elbow: the upper arm's child, the forearm (bending is less x)
        const entity elbow = zox_valid(child) ? bone_child_bone(world, child) : 0;
        const float more = fmaxf(0, -swing);   // the arm ahead
        walk_bend(world, elbow, -blend * (walk_elbow_bend + walk_elbow_lift * more), t);
    }
}
