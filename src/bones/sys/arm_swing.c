// The walk's limbs, in step: each posed from its skeleton's SwingPhase and SwingBlend (arm_control.c)
//  - legs swing from the hips (SwingAngle each way), the one swinging ahead bends its knee to lift its foot, straight
//    again as it lands; arms swing against them, elbows a little bent, more as the arm comes ahead
//  - standing (SwingBlend 0) the legs are still, the arms sway a little
//  - arms slerp towards their target, smoothing any other snaps (i.e. shoulder raise ending), legs are on it (the
//    blend eases them); the elbows and knees are the walk's while it swings their limb (arm_ik, leg_ik leave them be
//    at weight 0)
//  - crouched (CrouchBlend) the thighs tip ahead and the knees bend back twice as far, so the feet stay under the
//    hips (the body lowered by arm_control.c), the feet turned back level; the walk swings on top
//  - x turns a limb: less ahead, more back (a shin's more bends its knee, a forearm's less its elbow)
#define walk_follow_rate 18.0f     // how fast an arm slerps to its swing
#define walk_idle_arm_angle 5.0f    // degrees each way, standing
#define walk_knee_lift 55.0f        // the knee's most bend, mid swing ahead
#define walk_knee_bend 4.0f         // and always, walking
#define walk_elbow_bend 12.0f       // the elbow's bend, walking
#define walk_elbow_lift 18.0f       // more as the arm swings ahead
#define crouch_angle 55.0f          // crouched, the thighs ahead, the knees twice it (the feet under the hips), degrees
#define crouch_arms 15.0f           // and the arms a little ahead, elbows bent more
#define crouch_knee_lift 0.4f       // crouched, of the step's knee lift (bent already, it folded the leg up, 168 degrees)

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

// a leg's walk this frame, degrees (x: less ahead, more back): its thigh's turn and its knee's bend
//  - its side (x of its place: the right's -x) swings it against the other
// lifted: the knee's lift as it swings ahead (posing the leg); without it, the leg as it carries the body (its bob:
//  the lifted swinging leg counted as the longer one for a moment each step, two bumps a step, shaky walking)
static inline void walk_leg_angles(float swing_angle, float side_x, float phase, float blend, float crouch, byte lifted,
    float* thigh, float* knee)
{
    const float sign = side_x < 0 ? 1 : -1;
    const float ahead = -cosf(phase) * sign;    // > 0 swinging ahead (x falling)
    const float crouched = crouch * crouch_angle;
    const float lift = lifted ? walk_knee_lift * (1 - (1 - crouch_knee_lift) * crouch) : 0;
    *thigh = sinf(phase) * sign * swing_angle * blend - crouched;
    *knee = 2 * crouched + blend * (walk_knee_bend + lift * fmaxf(0, ahead));
}

// a limb's walk pose this frame: phase and blend its skeleton's (arm_control.c), limb 1 a leg
//  - posed by arm_control_system with its skeleton, on its thread: as systems of their own the limbs were split
//    across workers and some read the skeleton's phase before it moved (one leg a frame behind the other)
static inline void walk_limb(ecs* world, entity e, float phase, float blend, float crouch, byte limb, float follow_t) {
    const entity child = bone_child_bone(world, e);
    if (limb) {
        // legs on their swing, not eased: the body bobs from it this frame (arm_control.c), eased they lagged it (at
        //  10 frames a second a whole step) and the stance foot rose and sank 8cm
        float thigh, knee;
        walk_leg_angles(zox_getv(e, SwingAngle), zox_getv(e, LocalPosition3D).x, phase, blend, crouch, 1, &thigh, &knee);
        zox_getm(e, LocalRotation3D)->value = euler_to_quaternion((float3) { thigh * degrees_to_radians, 0, 0 });
        // the knee: its shin, and its foot level again from the crouch
        walk_bend(world, child, knee, 1);
        walk_bend(world, zox_valid(child) ? bone_child_bone(world, child) : 0, -crouch * crouch_angle, 1);
        return;
    }
    // arms against the legs, a little sway standing, eased (smoothing a raise ending)
    const float angle = walk_idle_arm_angle + ((float) zox_getv(e, SwingAngle) - walk_idle_arm_angle) * blend;
    const float swing = sinf(phase) * (zox_getv(e, LocalPosition3D).x < 0 ? -1 : 1);
    const float crouched = crouch * crouch_arms;
    const float4 target = euler_to_quaternion((float3) { (swing * angle - crouched) * degrees_to_radians, 0, 0 });
    LocalRotation3D* rotation = zox_getm(e, LocalRotation3D);
    rotation->value = quaternion_normalize2(quaternion_slerp(rotation->value, target, follow_t));
    // the elbow: the upper arm's child, the forearm (bending is less x)
    const entity elbow = zox_valid(child) ? bone_child_bone(world, child) : 0;
    const float more = fmaxf(0, -swing);   // the arm ahead
    walk_bend(world, elbow, -blend * (walk_elbow_bend + walk_elbow_lift * more) - 2 * crouched, follow_t);
}
